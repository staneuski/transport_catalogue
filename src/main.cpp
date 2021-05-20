#include <cassert>
#include <string>

#include "input_reader.h"
#include "stat_reader.h"
#include "transport_catalogue.h"

int main() {
    using namespace std;

    TransportCatalogue transport_catalogue;
    Fill(transport_catalogue);
    Search(transport_catalogue);

    return 0;
}