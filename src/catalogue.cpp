#include "catalogue.h"

#include <algorithm>

namespace transport {

using domain::Bus, domain::BusPtr, domain::BusLine;
using domain::Stop, domain::StopPtr, domain::StopStat;

// ---------- Catalogue ---------------

void Catalogue::AddStop(Stop stop) {
    stops_.push_back(std::move(stop));
    const StopPtr& stop_ptr = std::make_shared<const Stop>(stops_.back());

    stop_names_[stops_.back().name] = stop_ptr;
    stop_to_buses_[stop_ptr];
}

void Catalogue::MakeAdjacent(const StopPtr& stop,
                             const StopPtr& adjacent_stop,
                             const int metres) {
    const auto it = stops_to_distance_.find({adjacent_stop, stop});
    if (it != stops_to_distance_.end() && it->second == metres)
        return;

    stops_to_distance_[{stop, adjacent_stop}] = metres;
}

void Catalogue::AddBus(Bus bus) {
    buses_.push_back(std::move(bus));
    const BusPtr& bus_ptr = std::make_shared<const Bus>(buses_.back());

    bus_names_[buses_.back().name] = bus_ptr;
    for (const StopPtr& stop_ptr : buses_.back().stops)
        stop_to_buses_.at(stop_ptr).insert(bus_ptr);
}

std::optional<BusLine> Catalogue::GetBusLine(
    const std::string_view bus_name
) const {
    const BusPtr& bus_ptr = SearchBus(bus_name);
    if (!bus_ptr)
        return std::nullopt;

    BusLine bus_line;
    bus_line.ptr = bus_ptr;

    const std::vector<StopPtr>& stops = bus_ptr->stops;
    bus_line.stops_count = stops.size();

    const std::unordered_set<StopPtr> unique_stops{stops.begin(), stops.end()};
    bus_line.unique_stop_count = unique_stops.size();

    double distance = 0;
    const auto compute_bus_line = [&](StopPtr stop, StopPtr next_stop) {
        distance += domain::ComputeDistance(stop, next_stop);
        if (stops_to_distance_.find({stop, next_stop}) != stops_to_distance_.end())
            bus_line.length += stops_to_distance_.at({stop, next_stop});
        else if (stops_to_distance_.find({next_stop, stop}) != stops_to_distance_.end())
            bus_line.length += stops_to_distance_.at({next_stop, stop});
        else
            bus_line.length -= 1;
    };

    for (auto it = stops.begin(); it + 1 != stops.end(); ++it)
        compute_bus_line(*it, *std::next(it));

    if (!bus_ptr->is_roundtrip) {
        bus_line.stops_count = 2*bus_line.stops_count - 1;
        for (auto it = stops.rbegin(); it + 1 != stops.rend(); ++it)
            compute_bus_line(*it, *std::next(it));
    }
    bus_line.curvature = bus_line.length/distance;

    return bus_line;
}

std::optional<domain::StopStat> Catalogue::GetStop(
    const std::string_view stop_name
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

domain::SetStat<BusLine> Catalogue::GetAllBusLines() const {
    domain::SetStat<BusLine> bus_lines;
    for (const Bus& bus : buses_)
        bus_lines.insert(*GetBusLine(bus.name));
    return bus_lines;
}

domain::SetStat<StopStat> Catalogue::GetAllStopStats() const {
    domain::SetStat<StopStat> stop_stats;
    for (const auto& [stop_ptr, buses] : stop_to_buses_)
        if (!buses.empty())
            stop_stats.insert(*GetStop(stop_ptr->name));
    return stop_stats;
}

} // namespace transport