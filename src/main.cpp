#include <cassert>
#include <iomanip>
#include <string>

#include "input_reader.h"
#include "stat_reader.h"
#include "transport_catalogue.h"

TransportCatalogue InitialiseCatalogue() {
    TransportCatalogue transport_catalogue;

    transport_catalogue.AddStop({"A", {55.611087, 37.208290}});
    transport_catalogue.AddStop({"B", {55.595884, 37.209755}});
    transport_catalogue.AddStop({"C", {55.632761, 37.333324}});
    transport_catalogue.AddStop({"D", {55.574371, 37.651700}});
    transport_catalogue.AddStop({"E", {55.581065, 37.648390}});
    transport_catalogue.AddStop({"F", {55.587655, 37.645687}});
    transport_catalogue.AddStop({"G", {55.592028, 37.653656}});
    transport_catalogue.AddStop({"H", {55.580999, 37.659164}});

    transport_catalogue.AddBus(256, true, {"D", "E", "F", "G", "H", "D"});
    transport_catalogue.AddBus(750, false, {"A", "B", "C"});

    return transport_catalogue;
}

int main() {
    using namespace std;

    const TransportCatalogue transport_catalogue{InitialiseCatalogue()};
    std::cout << transport_catalogue.GetRoute(750) << endl;

    return 0;
}