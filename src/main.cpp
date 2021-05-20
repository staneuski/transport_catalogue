#include <string>

#include "input_reader.h"
#include "transport_catalogue.h"

int main() {
    using namespace std;

    TransportCatalogue transport_catalogue;
    transport_catalogue.AddStop({"Tolstopaltsevo", {55.611087, 37.208290}});
    cout << transport_catalogue.GetStop("Tolstopaltsevo")->name << endl;

    return 0;
}