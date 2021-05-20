#include "transport_catalogue.h"

void TransportCatalogue::AddBus(const int number,
                                const std::vector<std::string>& route) {
    std::vector<const Stop*> stops;
    stops.reserve(route.size());
    std::for_each(
        route.begin(),
        route.front() == route.back() ? std::prev(route.end()) : route.end(),
        [this, &stops](const std::string& stop_name) {
            stops.push_back(GetStop(stop_name));
        }
    );

    AddBus({number, stops});
}