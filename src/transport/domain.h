#pragma once
#include "geo/geo.h"

#include <memory>
#include <set>
#include <string>
#include <vector>

namespace transport {

namespace domain {

// ---------- Stop --------------------

struct Stop {
    std::string name;
    geo::Coordinates coords;
};
using StopPtr = std::shared_ptr<const Stop>;

// ---------- Bus ---------------------

struct Bus {
    std::string name;
    bool is_circular;
    std::vector<StopPtr> stops;
};
using BusPtr = std::shared_ptr<const Bus>;

struct LessBusPtr {
    inline bool operator()(const BusPtr lhs, const BusPtr rhs) const {
        return lhs->name < rhs->name;
    }
};
using SetBusPtr = std::set<std::shared_ptr<const Bus>, domain::LessBusPtr>;

// ---------- Route -------------------

struct Route {
    std::string_view name;
    BusPtr ptr;
    size_t stops_count, unique_stops_count;
    int length = 0;
    double curvature = 1.;
};

// ---------- StopStat ----------------

struct StopStat {
    std::string_view name;
    StopPtr ptr;
    const SetBusPtr& unique_buses;
};

// ---------- helpers -----------------

inline double ComputeDistance(const StopPtr current, const StopPtr next) {
    return geo::ComputeDistance(current->coords, next->coords);
}

} // end namespace domain

} // end namespace transport