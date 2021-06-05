#include <cassert>
#include <fstream>

#include "transport/request_handler.h"
#include "transport/json_reader.h"
#include "transport/transport_catalogue.h"

int main() {
    using namespace std;
    using namespace transport::io;

    transport::TransportCatalogue transport_catalogue;

    Requests requests = LoadRequests(cin);
    Fill(transport_catalogue, requests);

    Search(transport_catalogue, requests);

    return 0;
}