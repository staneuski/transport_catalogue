#pragma once
#include "geo/geo.h"

#include <set>
#include <string>
#include <vector>

namespace transport {

namespace domain {

struct Stop {
    std::string name;
    geo::Coordinates coords;
};

struct Bus {
    std::string name;
    bool is_circular;
    std::vector<const domain::Stop*> stops;
};

struct LessBusPtr {
    inline bool operator()(const domain::Bus* lhs, const domain::Bus* rhs) const {
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

inline double ComputeDistance(const Stop* current, const Stop* next) {
    return geo::ComputeDistance(current->coords, next->coords);
}

} // end namespace domain

} // end namespace transport