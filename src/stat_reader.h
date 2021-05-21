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
    for (const Request& request : requests) {
        if (IsBus(request))
            std::cout << transport_catalogue.GetRoute(request.name) << std::endl;
        else if (IsStop(request))
            std::cout << transport_catalogue.GetStop(request.name) << std::endl;
    }
}