#include <cassert>
#include <string>

#include "input_reader.h"
#include "stat_reader.h"
#include "transport_catalogue.h"

transport::TransportCatalogue InitialiseCatalogue() {
    const std::vector<transport::io::Request> stop_requests{
        {"A", {"55.611087", "37.20829", "3900m to B"}, ", "},
        {"B", {"55.595884", "37.209755", "9900m to C", "100m to B"}, ", "},
        {"C", {"55.632761", "37.333324", "9500m to B"}, ", "},
        {"D", {"55.574371", "37.6517", "7500m to I", "1800m to E", "2400m to F"}, ", "},
        {"E", {"55.581065", "37.64839", "750m to F"}, ", "},
        {"F", {"55.587655", "37.645687", "5600m to I", "900m to G"}, ", "},
        {"G", {"55.592028", "37.653656", "1300m to H"}, ", "},
        {"H", {"55.580999", "37.659164", "1200m to D"}, ", "},
        {"I", {"55.595579", "37.605757"}, ", "},
        {"J", {"55.611678", "37.603831"}, ", "}
    };

    const std::vector<transport::io::Request> bus_requests{
        {"828", {"D", "F", "I", "D"}, " > "},
        {"256", {"D", "E", "F", "G", "H", "D"}, " > "},
        {"750", {"A", "B", "B", "C"}, " - "}
    };

    transport::TransportCatalogue transport_catalogue;
    for (const transport::io::Request& request : stop_requests)
        transport_catalogue.AddStop(request);
    for (const transport::io::Request& request : stop_requests)
        transport_catalogue.MakeAdjacent(request);
    for (const transport::io::Request& request : bus_requests)
        transport_catalogue.AddBus(request);

    return transport_catalogue;
}

int main(int argc, char* argv[]) {
    using namespace std;
    using namespace transport::io;

    if (argc <= 1 && !argv[1]) {
        transport::TransportCatalogue transport_catalogue;
        Fill(transport_catalogue);
        Search(transport_catalogue);
    } else {
        const vector<string> stop_names{"A", "B", "D", "E", "F", "H", "I"};
        const vector<string> bus_names{"828", "750", "256", "751"};

        transport::TransportCatalogue transport_catalogue{InitialiseCatalogue()};
        for (const auto& name : stop_names)
            cout << transport_catalogue.GetStop(name) << endl;
        for (const auto& name : bus_names)
            cout << transport_catalogue.GetRoute(name) << endl;
    }

    return 0;
}