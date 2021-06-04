#pragma once

#include <iostream>
#include <string>
#include <variant>
#include <vector>
#include <unordered_map>

namespace transport {

class TransportCatalogue;

namespace io {

struct Request {
    enum class Type { BUS, STOP, };

    struct Bus {
        std::string name;
        std::vector<std::string> stops;
        bool is_roundtrip;
    };

    struct Stop {
        std::string name;
        double latitude;
        double longitude;
        std::unordered_map<std::string, int> stop_to_distances;
    };

    struct Stat {
        int ip;
        Type type;
        std::string name;
    };
};

struct Requests {
    std::vector<Request::Bus> buses;
    std::vector<Request::Stop> stops;
    std::vector<Request::Stat> stats;
};

void Fill(TransportCatalogue& transport_catalogue, const Requests& requests);

} // end namespace io
} // end namespace transport