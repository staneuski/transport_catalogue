#include <cassert>
#include <fstream>

#include "transport/json_reader.h"
#include "transport/map_renderer.h"
#include "transport/request_handler.h"
#include "transport/transport_catalogue.h"

int main() {
    using namespace std;
    using namespace transport;

    catalogue::TransportCatalogue db;
    io::JsonReader reader(cin);
    io::Populate(db, reader);

    renderer::MapRenderer map_rendrer = reader.GenerateMapSettings();
    cout << map_rendrer.RenderMap(db.GetAllRoutes(), db.GetAllStopStats()) << endl;

    // io::RequestHandler handler{db, map_rendrer};
    // io::Search(handler, reader);

    return 0;
}