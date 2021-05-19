#pragma once
#include <deque>
#include <functional>
#include <string>
#include <vector>

#include "geo.h"

struct Stop {
    std::string name;
    Coordinates coordinates;
};

struct Bus {
    int no;
    std::vector<Stop> stops;
};

class TransportCatalogue {
public:
    inline void AddBus(Bus& bus) {
        buses_.push_back(std::move(bus));
    }

    inline void AddStop(Stop& stop) {
        stops_.push_back(std::move(stop));
    }

private:
    std::deque<Bus> buses_;
    std::deque<Stop> stops_;
};