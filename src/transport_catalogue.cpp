#include "transport_catalogue.h"

void TransportCatalogue::AddStop(Stop&& stop) {
    stops_.push_back(std::move(stop));
    stop_names_[stops_.back().name] = &stops_.back();
    stop_to_buses_[&stops_.back()];
}

void TransportCatalogue::AddBus(Bus&& bus) {
    buses_.push_back(std::move(bus));
    bus_names_[buses_.back().name] = &buses_.back();

    for (const Stop* stop_ptr : buses_.back().stops)
        stop_to_buses_.at(stop_ptr).insert(&buses_.back());
}

void TransportCatalogue::AddBus(const std::string& bus_name,
                                const bool is_circular,
                                const std::vector<std::string>& route) {
    std::vector<const Stop*> stops;
    stops.reserve(route.size());
    for (const std::string& stop_name : route)
        stops.push_back(SearchStop(stop_name));

    AddBus({bus_name, is_circular, stops});
}

Route TransportCatalogue::GetRoute(const std::string_view& bus_name) const {
    const Bus* bus_ptr{SearchBus(bus_name)};
    if (!bus_ptr)
        return {bus_name, bus_ptr, 0, 0, .0};

    const std::vector<const Stop*>& stops = bus_ptr->stops;
    const std::unordered_set<const Stop*> unique_stops_t{stops.begin(), stops.end()};

    double route_len = .0;
    for (size_t i = 0; i + 1 < stops.size(); ++i)
        route_len += ComputeDistance(stops.at(i)->coords, stops.at(i + 1u)->coords);

    return {
        bus_name,
        bus_ptr,
        unique_stops_t.size(),
        bus_ptr->is_circular ? stops.size() : (2*stops.size() - 1),
        bus_ptr->is_circular ? route_len : 2*route_len
    };
}

StopStat TransportCatalogue::GetStop(const std::string_view& stop_name) const {
    const static std::set<const Bus*, LessBusPtr> empty_stop;
    const Stop* stop_ptr{SearchStop(stop_name)};
    return {
        stop_name,
        stop_ptr,
        stop_ptr ? stop_to_buses_.at(stop_ptr) : empty_stop
    };
}