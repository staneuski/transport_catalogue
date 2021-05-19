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
    std::vector<std::string> stops_names;
};

class TransportCatalogue {
public:
    inline void AddBus(Bus& bus) {
        buses_.push_back(std::move(bus));
    }

    inline void AddStop(Stop& stop) {
        stops_.push_back(std::move(stop));
    }

    inline size_t CountBuses() {
        return buses_.size();
    }

    inline size_t CountStops() {
        return stops_.size();
    }

private:
    std::deque<Bus> buses_;
    std::deque<Stop> stops_;
};