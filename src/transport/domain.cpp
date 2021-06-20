#include "domain.h"

namespace transport {
namespace domain {

std::ostream& operator<<(std::ostream& out,
                         const std::optional<domain::Route>& route) {
    if (route)
        out << "\"curvature\": " << route->curvature
            << ", \"route_length\": " << route->length
            << ", \"stop_count\": " << route->stops_count
            << ", \"unique_stop_count\": " << route->unique_stop_count;
    else
        out << "\"error_message\": \"not found\"";

    return out;
}

std::ostream& operator<<(std::ostream& out,
                         const std::optional<StopStat>& stop_stat) {
    if (stop_stat) {
        out << "\"buses\": [";
        bool is_first = true;
        for (const auto& bus : stop_stat->unique_buses) {
            if (is_first)
                is_first = false;
            else
                std::cout << ", ";
            out << "\"" << bus->name << "\"";
        }
        out << ']';
    } else {
        out << "\"error_message\": \"not found\"";
    }

    return out;
}

} // end namespace domain
} // end namespace transport