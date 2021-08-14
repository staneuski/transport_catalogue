#include <cassert>
#include <fstream>

#include "json_reader.h"
#include "map_renderer.h"
#include "request_handler.h"
#include "serialization.h"

using namespace std::literals;

void PrintUsage(std::ostream& stream = std::cerr) {
    stream << "Usage: transport_catalogue [make_base|process_requests]\n"sv;
}

int main(int argc, char* argv[]) {
    using namespace transport;

    if (argc != 2) {
        PrintUsage();
        return 1;
    }

    const std::string_view mode(argv[1]);

    io::JsonReader reader(std::cin);
    transport::Catalogue db;

    if (mode == "make_base"sv) {
        io::Populate(db, reader);
        io::RequestHandler handler{db, reader.GenerateMapSettings()};

        std::ofstream ofs(reader.GetDatabaseFileName(), std::ios::binary);
        io::Bufferiser(handler).Serialize(ofs);

    } else if (mode == "process_requests"sv) {

        // process requests here

    } else {
        PrintUsage();
        return 1;
    }
    return 0;
}