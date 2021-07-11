#pragma once
#include "geo/geo.h"

#include <iomanip>
#include <iostream>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <vector>

namespace transport {
namespace domain {

template<typename Ptr>
struct Less {
    inline bool operator()(const Ptr& lhs, const Ptr& rhs) const {
        return lhs->name < rhs->name;
    }
};

template<typename Ptr>
using SetPtr = std::set<Ptr, Less<Ptr>>;

template<typename Stat>
struct LessStat {
    inline bool operator()(const Stat& lhs, const Stat& rhs) const {
        return lhs.ptr->name < rhs.ptr->name;
    }
};

template<typename Stat>
using SetStat = std::set<Stat, LessStat<Stat>>;

// ---------- Stop --------------------

struct Stop {
    std::string name;
    geo::Coordinates coords;
};
using StopPtr = std::shared_ptr<const Stop>;

// ---------- Bus ---------------------

struct Bus {
    std::string name;
    std::vector<StopPtr> stops;
    bool is_roundtrip;
};
using BusPtr = std::shared_ptr<const Bus>;

// ---------- BusTiming ---------------

struct BusTiming {
    int velocity; // [km/h]
    int wait_time; // [min]
};

// ---------- BusLine -------------------

struct BusLine {
    BusPtr ptr;
    size_t stops_count;
    size_t unique_stop_count;
    int length = 0;
    double curvature = 1.;
};

// ---------- StopStat ----------------

struct StopStat {
    StopPtr ptr;
    const SetPtr<BusPtr>& unique_buses;
};

// ---------- helpers -----------------

inline double ComputeDistance(const StopPtr current, const StopPtr next) {
    return geo::ComputeDistance(current->coords, next->coords);
}

} // end namespace domain
} // end namespace transport