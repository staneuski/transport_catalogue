#include <cassert>
#include <fstream>

#include "transport/request_handler.h"
#include "transport/json_reader.h"
#include "transport/transport_catalogue.h"

int main() {
    using namespace std;
    using namespace transport::io;

    transport::TransportCatalogue transport_catalogue;
    JsonReader reader(cin);
    Populate(transport_catalogue, reader);
    Search(transport_catalogue, reader);

    return 0;
}