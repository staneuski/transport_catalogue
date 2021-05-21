#include <cassert>
#include <string>

#include "input_reader.h"
#include "stat_reader.h"
#include "transport_catalogue.h"

TransportCatalogue InitialiseCatalogue() {
    const std::vector<Request> stop_requests{
        {"A", {"55.611087", "37.208290"}, ", "},
        {"B", {"55.595884", "37.209755"}, ", "},
        {"C", {"55.632761", "37.333324"}, ", "},
        {"D", {"55.574371", "37.651700"}, ", "},
        {"E", {"55.581065", "37.648390"}, ", "},
        {"F", {"55.587655", "37.645687"}, ", "},
        {"G", {"55.592028", "37.653656"}, ", "},
        {"H", {"55.580999", "37.659164"}, ", "},
        {"I", {"55.581102", "37.659203"}, ", "}
    };
    const std::vector<Request> bus_requests{
        {"828", {"D", "F", "H", "D"}, " > "},
        {"750", {"A", "B", "C"}, " - "},
        {"256", {"D", "E", "F", "G", "H", "D"}, " > "}
    };

    TransportCatalogue transport_catalogue;
    for (const Request& request : stop_requests)
        transport_catalogue.AddStop(request);
    for (const Request& request : bus_requests)
        transport_catalogue.AddBus(request);

    return transport_catalogue;
}

int main(int argc, char* argv[]) {
    using namespace std;

    if (argc <= 1 && !argv[1]) {
        TransportCatalogue transport_catalogue;
        Fill(transport_catalogue);
        Search(transport_catalogue);
    } else {
        const vector<string> stop_names{"A", "B", "D", "E", "F", "H", "I"};
        const vector<string> bus_names{"828", "750", "256"};

        TransportCatalogue transport_catalogue = InitialiseCatalogue();
        for (const auto& name : stop_names)
            std::cout << transport_catalogue.GetStop(name) << std::endl;
        for (const auto& name : bus_names)
            std::cout << transport_catalogue.GetRoute(name) << std::endl;
    }

    return 0;
}