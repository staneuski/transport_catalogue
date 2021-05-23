#include "transport_catalogue.h"

namespace transport {

using domain::Bus, domain::Stop;

void TransportCatalogue::AddStop(Stop stop) {
    stops_.push_back(std::move(stop));
    stop_names_[stops_.back().name] = &stops_.back();
    stop_to_buses_[&stops_.back()];
}

void TransportCatalogue::AddBus(Bus bus) {
    buses_.push_back(std::move(bus));
    bus_names_[buses_.back().name] = &buses_.back();

    for (const Stop* stop_ptr : buses_.back().stops)
        stop_to_buses_.at(stop_ptr).insert(&buses_.back());
}

void TransportCatalogue::AddBus(const domain::Request& request) {
    std::vector<const Stop*> stops;
    stops.reserve(request.contents.size());
    for (const std::string& stop_name : request.contents)
        stops.push_back(SearchStop(stop_name));

    AddBus({request.name, request.delimiter == " > ", stops});
}

void TransportCatalogue::MakeAdjacent(const Stop* stop,
                                  const Stop* adjacent_stop,
                                  const int metres) {
    const auto it = stops_to_distance_.find({adjacent_stop, stop});
    if (it != stops_to_distance_.end() && it->second == metres)
        return;

    stops_to_distance_[{stop, adjacent_stop}] = metres;
}

void TransportCatalogue::MakeAdjacent(const domain::Request& request,
                                      const std::string_view delimiter) {
    const Stop* stop = SearchStop(request.name);
    std::for_each(
        request.contents.begin() + 2,
        request.contents.end(),
        [&](const std::string& s) {
            const size_t pos = s.find(delimiter);
            MakeAdjacent(
                stop,
                SearchStop(s.substr(pos + delimiter.size())),
                std::stoi(s.substr(0, pos))
            );
        }
    );
}

domain::Route TransportCatalogue::GetRoute(const std::string_view& bus_name) const {
    domain::Route route;
    route.name = bus_name;
    route.ptr = SearchBus(bus_name);
    if (!route.ptr)
        return route;

    const std::vector<const Stop*>& stops = route.ptr->stops;
    route.stops_count = stops.size();

    const std::unordered_set<const Stop*> unique_stops{stops.begin(), stops.end()};
    route.unique_stops_count = unique_stops.size();

    double distance = 0;
    const auto ComputeRoute = [&](const Stop* stop, const Stop* next_stop) {
        distance += geo::ComputeDistance(stop->coords, next_stop->coords);
        route.length += (stops_to_distance_.find({stop, next_stop}) != stops_to_distance_.end())
                        ? stops_to_distance_.at({stop, next_stop})
                        : stops_to_distance_.at({next_stop, stop});
    };

    for (auto it = stops.begin(); it + 1 != stops.end(); ++it)
        ComputeRoute(*it, *std::next(it));

    if (!route.ptr->is_circular) {
        route.stops_count = 2*route.stops_count - 1;
        for (auto it = stops.rbegin(); it + 1 != stops.rend(); ++it)
            ComputeRoute(*it, *std::next(it));
    }
    route.curvature = route.length/distance;
    return route;
}

domain::StopStat TransportCatalogue::GetStop(
    const std::string_view& stop_name
) const
{
    const static std::set<const Bus*, domain::LessBusPtr> empty_stop;
    const Stop* stop_ptr{SearchStop(stop_name)};
    return {
        stop_name,
        stop_ptr,
        stop_ptr ? stop_to_buses_.at(stop_ptr) : empty_stop
    };
}

} // end namespace transport