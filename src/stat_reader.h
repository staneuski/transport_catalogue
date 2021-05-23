#pragma once
#include <iomanip>
#include <iostream>

#include "input_reader.h"
#include "transport_catalogue.h"

namespace transport {

std::ostream& operator<<(std::ostream& out, const domain::Route& route) {
    out << "Bus " << route.name;

    if (route.ptr)
        out << ": " << route.stops_count << " stops on route"
            << ", " << route.unique_stops_count << " unique stops"
            << ", " << route.length << " route length"
            << ", " << std::setprecision(6) << route.curvature << " curvature";
    else
        out << ": not found";

    return out;
}

std::ostream& operator<<(std::ostream& out, const domain::StopStat& stop_stat) {
    out << "Stop " << stop_stat.name;

    if (stop_stat.ptr && !stop_stat.unique_buses.empty()) {
        out << ": buses";
        for (const domain::Bus* bus : stop_stat.unique_buses)
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
    for (const Request& request : requests) {
        if (IsBus(request))
            std::cout << transport_catalogue.GetRoute(request.name) << std::endl;
        else if (IsStop(request))
            std::cout << transport_catalogue.GetStop(request.name) << std::endl;
    }
}

} // end namespace transport