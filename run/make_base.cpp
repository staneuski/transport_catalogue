#include <cassert>
#include <fstream>

#include "json_reader.h"
#include "map_renderer.h"
#include "request_handler.h"

int main() {
    using namespace std;
    using namespace transport;

    transport::Catalogue db;

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

    json::Print(io::Search(handler, reader), std::cout);
    std::cout << std::endl;

    return 0;
}