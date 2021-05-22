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

void TransportCatalogue::AddBus(const Request& request) {
    std::vector<const Stop*> stops;
    stops.reserve(request.contents.size());
    for (const std::string& stop_name : request.contents)
        stops.push_back(SearchStop(stop_name));

    AddBus({request.name, request.delimiter == " > ", stops});
}

void TransportCatalogue::AbutStops(const Request& request,
                                   const std::string_view delimiter) {
    const Stop* stop = SearchStop(request.name);
    std::for_each(
        request.contents.begin() + 2,
        request.contents.end(),
        [&](const std::string& s) {
            const size_t pos = s.find(delimiter);
            AbutStop(
                stop,
                SearchStop(s.substr(pos + delimiter.size())),
                std::stoi(s.substr(0, pos))
            );
        }
    );
}

Route TransportCatalogue::GetRoute(const std::string_view& bus_name) const {
    Route route;
    route.name = bus_name;
    route.ptr = SearchBus(bus_name);
    if (!route.ptr)
        return route;

    const std::vector<const Stop*>& stops = route.ptr->stops;
    route.stops_count = stops.size();

    const std::unordered_set<const Stop*> unique_stops{stops.begin(), stops.end()};
    route.unique_stops_count = unique_stops.size();

    for (size_t i = 0; i + 1 < stops.size(); ++i) {
        const Stop* stop = stops.at(i);
        const Stop* next_stop = stops.at(i + 1u);
        route.length += ComputeDistance(stop->coords, next_stop->coords);
    }

    if (!route.ptr->is_circular) {
        route.length *= 2;
        route.stops_count = 2*route.stops_count - 1;
    }
    return route;
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