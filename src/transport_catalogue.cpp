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