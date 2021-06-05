#include "transport_catalogue.h"

namespace transport {

using domain::Bus, domain::Stop, domain::BusPtr, domain::StopPtr;

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

void TransportCatalogue::AddBus(const io::Request::Bus& request) {
    std::vector<StopPtr> stops;
    stops.reserve(request.stops.size());
    for (const std::string& stop_name : request.stops)
        stops.push_back(SearchStop(stop_name));

    AddBus({request.name, stops, request.is_roundtrip});
}

void TransportCatalogue::MakeAdjacent(const StopPtr& stop,
                                      const StopPtr& adjacent_stop,
                                      const int metres) {
    const auto it = stops_to_distance_.find({adjacent_stop, stop});
    if (it != stops_to_distance_.end() && it->second == metres)
        return;

    stops_to_distance_[{stop, adjacent_stop}] = metres;
}

domain::Route TransportCatalogue::GetRoute(const std::string_view& bus_name,
                                           const int request_id) const {
    domain::Route route;
    route.request_id = request_id;
    route.name = bus_name;
    route.ptr = SearchBus(bus_name);
    if (!route.ptr)
        return route;

    const std::vector<StopPtr>& stops = route.ptr->stops;
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

    if (!route.ptr->is_roundtrip) {
        route.stops_count = 2*route.stops_count - 1;
        for (auto it = stops.rbegin(); it + 1 != stops.rend(); ++it)
            ComputeRoute(*it, *std::next(it));
    }
    route.curvature = route.length/distance;
    return route;
}

domain::StopStat TransportCatalogue::GetStop(
    const std::string_view& stop_name,
    const int request_id
) const
{
    const static domain::SetBusPtr empty_stop;
    const StopPtr stop_ptr{SearchStop(stop_name)};
    return {
        request_id,
        stop_name,
        stop_ptr,
        stop_ptr ? stop_to_buses_.at(stop_ptr) : empty_stop
    };
}

} // end namespace transport