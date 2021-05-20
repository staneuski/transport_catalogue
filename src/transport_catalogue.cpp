#include "transport_catalogue.h"

void TransportCatalogue::AddBus(const int number,
                                const std::vector<std::string>& route) {
    std::vector<const Stop*> stops;
    stops.reserve(route.size());
    for (const std::string& stop_name : route)
        stops.push_back(GetStop(stop_name));

    AddBus({number, stops});
}