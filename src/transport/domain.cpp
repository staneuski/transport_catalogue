#include "domain.h"

namespace transport {
namespace domain {

std::ostream& operator<<(std::ostream& out, const Route& route) {
    out << '{';

    if (route.ptr)
        out << "\"curvature\": " << std::setprecision(6) << route.curvature
            << ", \"request_id\": " << route.request_id
            << ", \"route_length\": " << route.length
            << ", \"stop_count\": " << route.stops_count
            << ", \"unique_stop_count\": " << route.unique_stop_count;
    else
        out << "\"request_id\": " << route.request_id
            << ", \"error_message\": " << "\"not found\"";

    return out << '}';
}

std::ostream& operator<<(std::ostream& out, const StopStat& stop_stat) {
    out << '{';

    if (stop_stat.ptr) {
        out << "\"buses\": [";
        bool is_first = true;
        for (const auto& bus : stop_stat.unique_buses) {
            if (is_first)
                is_first = false;
            else
                std::cout << ", ";
            out << "\"" << bus->name << "\"";
        }
        out << "], \"request_id\": " << stop_stat.request_id;
    } else {
        out << "\"request_id\": " << stop_stat.request_id
            << ", \"error_message\": " << "\"not found\"";
    }

    return out << '}';
}

} // end namespace domain
} // end namespace transport