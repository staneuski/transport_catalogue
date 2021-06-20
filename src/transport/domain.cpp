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
        const auto& print_bus = [&out](const BusPtr& bus) {
            out << "\"" << bus->name << "\"";
        };

        out << "\"buses\": [";

        const SetPtr<BusPtr>& buses = stop_stat->unique_buses;
        print_bus(*buses.begin());
        std::for_each(
            std::next(buses.begin()), buses.end(),
            [&out, &print_bus](const BusPtr& bus) {
                out << ", ";
                print_bus(bus);
            }
        );

        out << ']';
    } else {
        out << "\"error_message\": \"not found\"";
    }

    return out;
}

} // end namespace domain
} // end namespace transport