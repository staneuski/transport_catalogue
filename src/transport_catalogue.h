#pragma once
#include <deque>
#include <functional>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <iostream>

#include "geo.h"

struct Request {
    std::string name;
    std::vector<std::string> contents;
    std::string delimiter;
};

struct Stop {
    std::string name;
    geo::Coordinates coords;
};

struct Bus {
    std::string name;
    bool is_circular;
    std::vector<const Stop*> stops;
};

struct LessBusPtr {
    inline bool operator()(const Bus* lhs, const Bus* rhs) const {
        return lhs->name < rhs->name;
    }
};

struct Route {
    std::string_view name;
    const Bus* ptr;
    size_t stops_count, unique_stops_count;
    int length = 0;
    double curvature = 1.;
};

struct StopStat {
    std::string_view name;
    const Stop* ptr;
    const std::set<const Bus*, LessBusPtr>& unique_buses;
};

class TransportCatalogue {
    using AdjacentStops = std::pair<const Stop*, const Stop*>;

    class AdjacentStopsHasher {
    public:
        inline size_t operator()(const AdjacentStops adjacent_stops) const {
            return hash_(adjacent_stops.first) + hash_(adjacent_stops.first)*37;
        }
    private:
        std::hash<const void*> hash_;
    };

public:
    void AddStop(Stop&& stop);

    inline void AddStop(const Request& request) {
        AddStop({
            request.name,
            {std::stod(request.contents[0]), std::stod(request.contents[1])}
        });
    }

    void AddBus(Bus&& bus);

    void AddBus(const Request& request);

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

    void AbutStop(const Stop* stop,
                  const Stop* adjacent_stop,
                  const int distance);

    void AbutStops(const Request& request,
                   const std::string_view delimiter = "m to ");

    Route GetRoute(const std::string_view& bus_name) const;

    StopStat GetStop(const std::string_view& stop_name) const;

private:
    std::deque<Stop> stops_;
    std::deque<Bus> buses_;
    std::unordered_map<std::string_view, const Stop*> stop_names_;
    std::unordered_map<std::string_view, const Bus*> bus_names_;
    std::unordered_map<const Stop*, std::set<const Bus*, LessBusPtr>> stop_to_buses_;
    std::unordered_map<AdjacentStops, int, AdjacentStopsHasher> stops_to_distance_;
};