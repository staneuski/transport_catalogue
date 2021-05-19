#include <string>

#include "input_reader.h"
#include "transport_catalogue.h"

int main() {
    using namespace std;

    TransportCatalogue transport_catalogue;

    FillFromInput(ReadLineWithNumber(), transport_catalogue);

    cout << "{ stops: " << transport_catalogue.CountStops()
         << ", buses: " << transport_catalogue.CountBuses()
         << " } " << endl;

    return 0;
}