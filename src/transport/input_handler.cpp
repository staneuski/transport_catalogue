#include "input_handler.h"

#include "json_reader.h"
#include "transport_catalogue.h"

namespace transport {
namespace io {

void Fill(TransportCatalogue& transport_catalogue, const Requests& requests) {
    for (const io::StopRequest& request : requests.stops)
        transport_catalogue.AddStop(request);
    for (const io::StopRequest& request : requests.stops)
        transport_catalogue.MakeAdjacent(request);

    for (const io::BusRequest& request : requests.buses)
        transport_catalogue.AddBus(request);
}
} // end namespace io
} // end namespace transport