#include <cassert>

#include "transport/input_handler.h"
#include "transport/json_reader.h"

int main() {
    using namespace std;
    using namespace transport::io;

    const Requests requests = LoadRequests(cin);

    cout << requests.buses.front().name << endl;

    return 0;
}