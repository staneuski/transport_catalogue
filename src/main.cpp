#include <cassert>
#include <fstream>

#include "transport/json_reader.h"
#include "transport/request_handler.h"
#include "transport/transport_catalogue.h"

int main() {
    using namespace std;
    using namespace transport::io;

    transport::TransportCatalogue transport_catalogue;
    JsonReader reader(cin);
    Populate(transport_catalogue, reader);

    RequestHandler handler{transport_catalogue};
    Search(handler, reader);

    return 0;
}