#pragma once

#include <iostream>
#include <string>
#include <variant>
#include <vector>
#include <unordered_map>

namespace transport {

class TransportCatalogue;

namespace io {

enum class RequestType { BUS, STOP, };

struct BusRequest {
    std::string name;
    std::vector<std::string> stops;
    bool is_roundtrip;
};

struct StopRequest {
    std::string name;
    double latitude;
    double longitude;
    std::unordered_map<std::string, int> stop_to_distances;
};

struct StatRequest {
    int ip;
    RequestType type;
    std::string name;
};

struct Requests {
    std::vector<BusRequest> buses;
    std::vector<StopRequest> stops;
    std::vector<StatRequest> stats;
};

void Fill(TransportCatalogue& transport_catalogue, const Requests& requests);

} // end namespace io
} // end namespace transport