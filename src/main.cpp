#include <cassert>
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
    transport_catalogue.AddStop({"I", {55.580999, 37.659164}});

    transport_catalogue.AddBus("256", true, {"D", "E", "F", "G", "H", "D"});
    transport_catalogue.AddBus("750", false, {"A", "B", "C"});
    transport_catalogue.AddBus("828", true, {"D", "F", "H", "D"});

    return transport_catalogue;
}

int main() {
    using namespace std;

    // TransportCatalogue transport_catalogue = InitialiseCatalogue();
    // cout << transport_catalogue.GetStop("I") << endl;

    TransportCatalogue transport_catalogue;
    Fill(transport_catalogue);
    Search(transport_catalogue);

    return 0;
}