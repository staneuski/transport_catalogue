#pragma once
#include <deque>
#include <functional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "geo.h"

struct Stop {
    std::string name;
    Coordinates coords;
};

struct Bus {
    std::string name;
    bool is_circular;
    std::vector<const Stop*> stops;
};

struct Route {
    std::string name;
    const Bus* bus_ptr;
    size_t unique_stops, stops;
    double length;
};

class TransportCatalogue {
public:
    inline void AddStop(Stop&& stop) {
        stops_.push_back(std::move(stop));
        stop_names_[stops_.back().name] = &stops_.back();
    }

    inline void AddBus(Bus&& bus) {
        buses_.push_back(std::move(bus));
        bus_names_[buses_.back().name] = &buses_.back();
    }

    void AddBus(const std::string& bus_name,
                const bool is_circular,
                const std::vector<std::string>& route);

    inline const Stop* SearchStop(const std::string_view& stop_name) const {
        return (stop_names_.find(stop_name) != stop_names_.end())
               ? stop_names_.at(stop_name)
               : nullptr;
    }

    inline const Bus* SearchBus(const std::string_view& bus_name) const {
        return (bus_names_.find(bus_name) != bus_names_.end())
               ? bus_names_.at(bus_name)
               : nullptr;
    }

    Route GetRoute(const std::string& name) const;

private:
    std::deque<Stop> stops_;
    std::deque<Bus> buses_;
    std::unordered_map<std::string_view, const Stop*> stop_names_;
    std::unordered_map<std::string_view, const Bus*> bus_names_;
};