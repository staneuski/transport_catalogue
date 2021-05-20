#pragma once
#include <deque>
#include <functional>
#include <string>
#include <map>
#include <unordered_map>
#include <vector>

#include "geo.h"

struct Stop {
    std::string name;
    Coordinates coords;
};

struct Bus {
    int no;
    std::vector<const Stop*> stops;
};

class TransportCatalogue {
public:
    inline void AddStop(Stop&& stop) {
        stops_.push_back(std::move(stop));
        stop_names_[stops_.back().name] = &stops_.back();
    }

    inline void AddBus(Bus&& bus) {
        buses_.push_back(std::move(bus));
        bus_numbers_[buses_.back().no] = &buses_.back();
    }

    void AddBus(const int number, const std::vector<std::string>& route);

    inline const Stop* SearchStop(const std::string& stop_name) {
        return (stop_names_.find(stop_name) != stop_names_.end())
               ? stop_names_.at(stop_name)
               : nullptr;
    }

    inline const Bus* SearchBus(const int bus_number) {
        return (bus_numbers_.find(bus_number) != bus_numbers_.end())
               ? bus_numbers_.at(bus_number)
               : nullptr;
    }

private:
    std::deque<Stop> stops_;
    std::deque<Bus> buses_;
    std::unordered_map<std::string_view, const Stop*> stop_names_;
    std::unordered_map<int, const Bus*> bus_numbers_;
};