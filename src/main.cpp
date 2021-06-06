#include <cassert>
#include <fstream>

#include "transport/json_reader.h"
#include "transport/map_renderer.h"
#include "transport/request_handler.h"
#include "transport/transport_catalogue.h"

int main() {
    using namespace std;
    using namespace transport;

    catalogue::TransportCatalogue transport_catalogue;
    io::JsonReader reader(cin);
    io::Populate(transport_catalogue, reader);

    renderer::MapRenderer map_rendrer = reader.GenerateMapSettings();
    cout << map_rendrer.RenderMap() << endl;

    // io::RequestHandler handler{transport_catalogue, map_rendrer};
    // io::Search(handler, reader);

    return 0;
}