#include <cassert>
#include <fstream>

#include "transport/catalogue.h"
#include "transport/json_reader.h"
#include "transport/map_renderer.h"
#include "transport/request_handler.h"

int main() {
    using namespace std;
    using namespace transport;

    catalogue::TransportCatalogue db;

    io::JsonReader reader(cin);
    io::Populate(db, reader);

    // std::ifstream file("../tests/input-rand.json");
    // std::stringstream buffer;
    // buffer << file.rdbuf();
    // io::JsonReader reader(buffer);
    // io::Populate(db, reader);

    renderer::MapRenderer map_renderer = reader.GenerateMapSettings();
    // cout << map_renderer.RenderMap(db.GetAllBusLines(), db.GetAllStopStats()) << endl;

    io::RequestHandler handler{db, map_renderer};
    io::Search(handler, reader);

    return 0;
}