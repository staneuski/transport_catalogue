#include "transport_catalogue.h"

void TransportCatalogue::AddBus(const int number,
                                const bool is_circular,
                                const std::vector<std::string>& route) {
    std::vector<const Stop*> stops;
    stops.reserve(route.size());
    for (const std::string& stop_name : route)
        stops.push_back(SearchStop(stop_name));

    AddBus({number, is_circular, stops});
}

Route TransportCatalogue::GetRoute(const int bus_number) const {
    const Bus* bus_ptr{SearchBus(bus_number)};
    if (!bus_ptr)
        return {bus_ptr, bus_number, 0, 0, .0};

    const std::vector<const Stop*>& stops = bus_ptr->stops;
    const std::unordered_set<const Stop*> unique_stops{stops.begin(), stops.end()};

    double route_len = .0;
    for (size_t i = 0; i + 1 < stops.size(); ++i)
        route_len += ComputeDistance(stops.at(i)->coords, stops.at(i + 1u)->coords);

    return {
        bus_ptr,
        bus_number,
        unique_stops.size(),
        bus_ptr->is_circular ? stops.size() : (2*stops.size() - 1),
        bus_ptr->is_circular ? route_len : 2*route_len
    };
}