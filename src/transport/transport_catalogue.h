#pragma once

#include <deque>
#include <functional>
#include <unordered_map>
#include <unordered_set>

#include "domain.h"

namespace transport {
namespace catalogue {

class TransportCatalogue {
    using AdjacentStops = std::pair<domain::StopPtr, domain::StopPtr>;

    class AdjacentStopsHasher {
    public:
        inline size_t operator()(const AdjacentStops adjacent_stops) const {
            return hash_(adjacent_stops.first.get())
                 + hash_(adjacent_stops.first.get())*37;
        }
    private:
        std::hash<const void*> hash_;
    };

public:
    void AddStop(domain::Stop stop);

    void AddBus(domain::Bus bus);

    inline domain::StopPtr SearchStop(const std::string_view& stop_name) const {
        return (stop_names_.find(stop_name) != stop_names_.end())
               ? stop_names_.at(stop_name)
               : nullptr;
    }

    inline domain::BusPtr SearchBus(const std::string_view& bus_name) const {
        return (bus_names_.find(bus_name) != bus_names_.end())
               ? bus_names_.at(bus_name)
               : nullptr;
    }

    void MakeAdjacent(const domain::StopPtr& stop,
                      const domain::StopPtr& adjacent_stop,
                      const int distance);

    domain::Route GetRoute(const std::string_view& bus_name) const;

    domain::StopStat GetStop(const std::string_view& stop_name) const;

private:
    std::deque<domain::Stop> stops_;
    std::deque<domain::Bus> buses_;
    std::unordered_map<std::string_view, domain::StopPtr> stop_names_;
    std::unordered_map<std::string_view, domain::BusPtr> bus_names_;
    std::unordered_map<domain::StopPtr, domain::SetPtr<domain::BusPtr>> stop_to_buses_;
    std::unordered_map<AdjacentStops, int, AdjacentStopsHasher> stops_to_distance_;
};

} // end namespace catalogue
} // end namespace transport