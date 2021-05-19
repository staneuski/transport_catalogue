#pragma once
#include <deque>
#include <functional>
#include <string>
#include <vector>

#include "geo.h"

struct Stop {
    std::string name;
    Coordinates coords;
};

struct Bus {
    int no;
    std::vector<std::string> stops;
};

class TransportCatalogue {
public:
    inline void AddStop(Stop&& stop) {
        stops_.push_back(std::move(stop));
    }

    inline void AddBus(Bus&& bus) {
        buses_.push_back(std::move(bus));
    }

private:
    std::deque<Stop> stops_;
    std::deque<Bus> buses_;
};