#include "transport_catalogue.h"

namespace transport {
namespace catalogue {

using domain::Bus, domain::BusPtr, domain::Route;
using domain::Stop, domain::StopPtr, domain::StopStat;

// ---------- TransportCatalogue ------

void TransportCatalogue::AddStop(Stop stop) {
    stops_.push_back(std::move(stop));
    const StopPtr& stop_ptr = std::make_shared<const Stop>(stops_.back());

    stop_names_[stops_.back().name] = stop_ptr;
    stop_to_buses_[stop_ptr];
}

void TransportCatalogue::AddBus(Bus bus) {
    buses_.push_back(std::move(bus));
    const BusPtr& bus_ptr = std::make_shared<const Bus>(buses_.back());

    bus_names_[buses_.back().name] = bus_ptr;
    for (const StopPtr& stop_ptr : buses_.back().stops)
        stop_to_buses_.at(stop_ptr).insert(bus_ptr);
}

void TransportCatalogue::MakeAdjacent(const StopPtr& stop,
                                      const StopPtr& adjacent_stop,
                                      const int metres) {
    const auto it = stops_to_distance_.find({adjacent_stop, stop});
    if (it != stops_to_distance_.end() && it->second == metres)
        return;

    stops_to_distance_[{stop, adjacent_stop}] = metres;
}

std::optional<Route> TransportCatalogue::GetRoute(
    const std::string_view& bus_name
) const {
    const BusPtr& bus_ptr = SearchBus(bus_name);
    if (!bus_ptr)
        return std::nullopt;

    Route route;
    route.ptr = bus_ptr;

    const std::vector<StopPtr>& stops = bus_ptr->stops;
    route.stops_count = stops.size();

    const std::unordered_set<StopPtr> unique_stops{stops.begin(), stops.end()};
    route.unique_stop_count = unique_stops.size();

    double distance = 0;
    const auto ComputeRoute = [&](StopPtr stop, StopPtr next_stop) {
        distance += domain::ComputeDistance(stop, next_stop);
        route.length += (stops_to_distance_.find({stop, next_stop}) != stops_to_distance_.end())
                        ? stops_to_distance_.at({stop, next_stop})
                        : stops_to_distance_.at({next_stop, stop});
    };

    for (auto it = stops.begin(); it + 1 != stops.end(); ++it)
        ComputeRoute(*it, *std::next(it));

    if (!bus_ptr->is_roundtrip) {
        route.stops_count = 2*route.stops_count - 1;
        for (auto it = stops.rbegin(); it + 1 != stops.rend(); ++it)
            ComputeRoute(*it, *std::next(it));
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

domain::SetStat<Route> TransportCatalogue::GetAllRoutes() const {
    domain::SetStat<Route> routes;
    for (const Bus& bus : buses_)
        routes.insert(*GetRoute(bus.name));
    return routes;
}

} // end namespace catalogue
} // end namespace transport