#pragma once
#include <iomanip>
#include <iostream>

#include "input_reader.h"
#include "transport_catalogue.h"

std::ostream& operator<<(std::ostream& out, const Route& route) {
    out << "Bus " << route.name;

    if (route.ptr)
        out << ": " << route.stops_count << " stops on route"
            << ", " << route.unique_stops_count << " unique stops"
            << ", " << std::setprecision(6) << route.length << " route length";
    else
        out << ": not found";

    return out;
}

std::ostream& operator<<(std::ostream& out, const StopStat& stop_stat) {
    out << "Stop " << stop_stat.name;

    if (stop_stat.ptr && !stop_stat.unique_buses.empty()) {
        out << ": buses";
        for (const Bus* bus : stop_stat.unique_buses)
            out << ' ' << bus->name;
    } else if (stop_stat.ptr && stop_stat.unique_buses.empty()) {
        out << ": no buses";
    } else {
        out << ": not found";
    }

    return out;
}

void Search(const TransportCatalogue& transport_catalogue) {
    std::vector<Request> requests{ReadRequests()};
    for (Request& request : requests) {
        if (request.first.substr(0, 3) == "Bus") {
            const auto& [number, is_circular, _] = ParseBus(request);
            std::cout << transport_catalogue.GetRoute(number) << std::endl;
        } else if (request.first.substr(0, 4) == "Stop") {
            std::cout << transport_catalogue.GetStop(ParseStop(request).name)
                      << std::endl;
        }
    }
}