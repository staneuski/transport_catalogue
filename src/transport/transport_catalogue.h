#pragma once
#include "domain.h"

#include <deque>
#include <functional>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "request_handler.h"
// #include "json_reader.h"

namespace transport {

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

    inline void AddStop(const io::Request::Stop& request) {
        AddStop({
            request.name,
            {request.latitude, request.longitude}
        });
    }

    void AddBus(domain::Bus bus);

    void AddBus(const io::Request::Bus& request);

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

    inline void MakeAdjacent(const io::Request::Stop& request) {
        domain::StopPtr stop_ptr = SearchStop(request.name);
        for (const auto& [stop_name, distance] : request.stop_to_distances)
            MakeAdjacent(stop_ptr, SearchStop(stop_name), distance);
    }

    domain::Route GetRoute(const std::string_view& bus_name,
                           const int request_id = 0) const;

    domain::StopStat GetStop(const std::string_view& stop_name,
                             const int request_id = 0) const;

private:
    std::deque<domain::Stop> stops_;
    std::deque<domain::Bus> buses_;
    std::unordered_map<std::string_view, domain::StopPtr> stop_names_;
    std::unordered_map<std::string_view, domain::BusPtr> bus_names_;
    std::unordered_map<domain::StopPtr, domain::SetBusPtr> stop_to_buses_;
    std::unordered_map<AdjacentStops, int, AdjacentStopsHasher> stops_to_distance_;
};

} // end namespace transport