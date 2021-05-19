#include <iostream>
#include <string>

#include "transport_catalogue.h"

int main() {
    using namespace std;

    Stop stop{"Tolstopaltsevo", {55.611087, 37.208290}};

    TransportCatalogue transport_catalogue;

    transport_catalogue.AddStop(stop);

    return 0;
}