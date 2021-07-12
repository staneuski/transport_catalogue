#include "catalogue.h"

#include <algorithm>

namespace transport {
namespace catalogue {

using domain::Bus, domain::BusPtr, domain::BusLine;
using domain::Stop, domain::StopPtr, domain::StopStat;

// ---------- TransportCatalogue ------

void TransportCatalogue::AddStop(Stop stop) {
    stops_.push_back(std::move(stop));
    const StopPtr& stop_ptr = std::make_shared<const Stop>(stops_.back());

    stop_names_[stops_.back().name] = stop_ptr;
    stop_to_buses_[stop_ptr];
}

void TransportCatalogue::MakeAdjacent(const StopPtr& stop,
                                      const StopPtr& adjacent_stop,
                                      const int metres) {
    const auto it = stops_to_distance_.find({adjacent_stop, stop});
    if (it != stops_to_distance_.end() && it->second == metres)
        return;

    stops_to_distance_[{stop, adjacent_stop}] = metres;
}

void TransportCatalogue::AddBus(Bus bus) {
    buses_.push_back(std::move(bus));
    const BusPtr& bus_ptr = std::make_shared<const Bus>(buses_.back());

    bus_names_[buses_.back().name] = bus_ptr;
    for (const StopPtr& stop_ptr : buses_.back().stops)
        stop_to_buses_.at(stop_ptr).insert(bus_ptr);
}

std::vector<StopPtr> TransportCatalogue::GetStops() const {
    std::vector<StopPtr> stops;
    std::transform(
        stop_names_.begin(),
        stop_names_.end(),
        std::back_inserter(stops),
        [](const auto& name_to_holder){ return name_to_holder.second; }
    );
    return stops;
}

std::vector<BusPtr> TransportCatalogue::GetBuses() const {
    std::vector<BusPtr> buses;
    std::transform(
        bus_names_.begin(),
        bus_names_.end(),
        std::back_inserter(buses),
        [](const auto& name_to_holder){ return name_to_holder.second; }
    );
    return buses;
}

std::optional<BusLine> TransportCatalogue::GetBusLine(
    const std::string_view& bus_name
) const {
    const BusPtr& bus_ptr = SearchBus(bus_name);
    if (!bus_ptr)
        return std::nullopt;

    BusLine route;
    route.ptr = bus_ptr;

    const std::vector<StopPtr>& stops = bus_ptr->stops;
    route.stops_count = stops.size();

    const std::unordered_set<StopPtr> unique_stops{stops.begin(), stops.end()};
    route.unique_stop_count = unique_stops.size();

    double distance = 0;
    const auto ComputeBusLine = [&](StopPtr stop, StopPtr next_stop) {
        distance += domain::ComputeDistance(stop, next_stop);
        if (stops_to_distance_.find({stop, next_stop}) != stops_to_distance_.end())
            route.length += stops_to_distance_.at({stop, next_stop});
        else if (stops_to_distance_.find({next_stop, stop}) != stops_to_distance_.end())
            route.length += stops_to_distance_.at({next_stop, stop});
        else
            route.length -= 1;
    };

    for (auto it = stops.begin(); it + 1 != stops.end(); ++it)
        ComputeBusLine(*it, *std::next(it));

    if (!bus_ptr->is_roundtrip) {
        route.stops_count = 2*route.stops_count - 1;
        for (auto it = stops.rbegin(); it + 1 != stops.rend(); ++it)
            ComputeBusLine(*it, *std::next(it));
    }
    route.curvature = route.length/distance;

    return route;
}

std::optional<domain::StopStat> TransportCatalogue::GetStop(
    const std::string_view& stop_name
) const {
    const static domain::SetPtr<BusPtr> empty_stop;

    const StopPtr& stop_ptr = SearchStop(stop_name);
    if (!stop_ptr)
        return std::nullopt;

    return StopStat{
        stop_ptr,
        stop_ptr ? stop_to_buses_.at(stop_ptr) : empty_stop
    };
}

domain::SetStat<BusLine> TransportCatalogue::GetAllBusLines() const {
    domain::SetStat<BusLine> routes;
    for (const Bus& bus : buses_)
        routes.insert(*GetBusLine(bus.name));
    return routes;
}

domain::SetStat<StopStat> TransportCatalogue::GetAllStopStats() const {
    domain::SetStat<StopStat> stop_stats;
    for (const auto& [stop_ptr, buses] : stop_to_buses_)
        if (!buses.empty())
            stop_stats.insert(*GetStop(stop_ptr->name));
    return stop_stats;
}

} // namespace catalogue
} // namespace transport