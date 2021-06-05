#include "input_handler.h"

#include "json_reader.h"
#include "transport_catalogue.h"

namespace transport {
namespace io {

void Fill(TransportCatalogue& transport_catalogue, const Requests& requests) {
    for (const Request::Stop& request : requests.stops)
        transport_catalogue.AddStop(request);
    for (const Request::Stop& request : requests.stops)
        transport_catalogue.MakeAdjacent(request);

    for (const io::Request::Bus& request : requests.buses)
        transport_catalogue.AddBus(request);
}

void Search(const TransportCatalogue& transport_catalogue,
            const Requests& requests) {
    std::cout << "[\n";

    bool is_first = true;
    for (const Request::Stat& request : requests.stats) {
        if (is_first)
            is_first = false;
        else
            std::cout << ",\n";
        std::cout << std::string(INDENT_SIZE, ' ');

        if (request.type == Request::Type::BUS)
            std::cout << transport_catalogue.GetRoute(request.name, request.id);
        else if (request.type == Request::Type::STOP)
            std::cout << transport_catalogue.GetStop(request.name, request.id);
    }

    std::cout << "\n]" << std::endl;
}

} // end namespace io
} // end namespace transport