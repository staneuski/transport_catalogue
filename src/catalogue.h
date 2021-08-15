#pragma once
#include "domain.h"

#include <deque>
#include <functional>
#include <optional>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace transport {

class Catalogue {
public:
    using AdjacentStops = std::pair<domain::StopPtr, domain::StopPtr>;

    class AdjacentStopsHasher {
    public:
        inline size_t operator()(const AdjacentStops adjacent_stops) const {
            return hash_(adjacent_stops.first.get())
                   + hash_(adjacent_stops.second.get())*37;
        }

    private:
        std::hash<const void*> hash_;
    };

public:
    inline size_t GetStopCount() const {
        return stops_.size();
    }

    inline size_t GetBusCount() const {
        return buses_.size();
    }

    inline const std::unordered_map<std::string_view, domain::StopPtr>&
    GetStopsHolder() const {
        return stop_names_;
    }

    inline const std::unordered_map<std::string_view, domain::BusPtr>&
    GetBusesHolder() const {
        return bus_names_;
    }

    inline const std::unordered_map<AdjacentStops, int, AdjacentStopsHasher>&
    GetDistances() const {
        return stops_to_distance_;
    }

    inline size_t GetStopId(const std::string_view stop_name) const {
        return std::distance(
            stops_.begin(),
            FindByName(stops_.begin(), stops_.end(), stop_name)
        );
    }

    inline size_t GetBusId(const std::string_view bus_name) const {
        return std::distance(
            buses_.begin(),
            FindByName(buses_.begin(), buses_.end(), bus_name)
        );
    }

    inline domain::StopPtr GetStop(const size_t id) const {
        return std::next(stop_names_.begin(), id)->second;
    }

    inline domain::BusPtr GetBus(const size_t id) const {
        return std::next(bus_names_.begin(), id)->second;
    }

    inline domain::StopPtr SearchStop(const std::string_view stop_name) const {
        return (stop_names_.find(stop_name) != stop_names_.end())
               ? stop_names_.at(stop_name)
               : nullptr;
    }

    inline domain::BusPtr SearchBus(const std::string_view bus_name) const {
        return (bus_names_.find(bus_name) != bus_names_.end())
               ? bus_names_.at(bus_name)
               : nullptr;
    }

    void AddStop(domain::Stop stop);

    void MakeAdjacent(const domain::StopPtr& stop,
                      const domain::StopPtr& adjacent_stop,
                      const int distance);

    void AddBus(domain::Bus bus);

    std::optional<domain::BusLine> GetBusLine(
        const std::string_view bus_name
    ) const;

    std::optional<domain::StopStat> GetStop(
        const std::string_view stop_name
    ) const;

    domain::SetStat<domain::BusLine> GetAllBusLines() const;

    domain::SetStat<domain::StopStat> GetAllStopStats() const;

private:
    std::deque<domain::Stop> stops_;
    std::deque<domain::Bus> buses_;
    std::unordered_map<std::string_view, domain::StopPtr> stop_names_;
    std::unordered_map<std::string_view, domain::BusPtr> bus_names_;
    std::unordered_map<domain::StopPtr, domain::SetPtr<domain::BusPtr>> stop_to_buses_;
    std::unordered_map<AdjacentStops, int, AdjacentStopsHasher> stops_to_distance_; //[m]

    template<typename InputIt>
    static InputIt FindByName(InputIt first, InputIt last, std::string_view name);
};

template<typename InputIt>
InputIt Catalogue::FindByName(InputIt first, InputIt last, std::string_view name) {
    return std::find_if(first, last, [name](const auto& element) {
        return name == element.name;
    });
}

} // namespace transport