#pragma once
#include "graph/routes.h"

#include <unordered_map>

#include "domain.h"

namespace transport {

class Graph : public graph::DirectedWeightedGraph<double> {
    using Transfer = std::pair<graph::VertexId, graph::VertexId>;
    using Distances = std::unordered_map<domain::AdjacentStops, int, domain::AdjacentStopsHasher>;

    struct Edge {
        domain::StopPtr from;
        domain::StopPtr to;
        domain::BusPtr bus;
        size_t stop_count;
        double timerange;
    };

public:
    explicit Graph(
        const std::vector<domain::StopPtr>& stops,
        const std::vector<domain::BusPtr>& buses,
        const Distances& stops_to_distance
    ) : graph::DirectedWeightedGraph<double>(2*stops.size()) {
        FillStopEdges(stops);
        FillBusEdges(buses, stops_to_distance);
    }

private:
    std::unordered_map<domain::StopPtr, Transfer> stop_to_transfer_;
    std::unordered_map<graph::EdgeId, Edge> edges_;

    void FillStopEdges(const std::vector<domain::StopPtr>& stops);

    void FillBusEdges(const std::vector<domain::BusPtr>& buses,
                      const Distances& stops_to_distance);
};

} // namespace transport
