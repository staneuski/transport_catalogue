#pragma once
#include <iomanip>
#include <iostream>

#include "input_reader.h"
#include "transport_catalogue.h"

std::ostream& operator<<(std::ostream& out, const Route& route) {
    out << "Bus " << route.bus_number;

    if (route.bus_ptr)
        out << ": " << route.stops << " stops on route"
            << ", " << route.unique_stops << " unique stops"
            << ", " << std::setprecision(6) << route.length << " route length";
    else
        out << ": not found";

    return out;
}

void Search(const TransportCatalogue& transport_catalogue) {
    std::vector<Request> requests{ReadRequests()};
    for (Request& request : requests)
        if (request.description.substr(0, 3) == "Bus") {
            const auto& [number, is_circular, _] = ParseBus(request);
            std::cout << transport_catalogue.GetRoute(number) << std::endl;
        }
}