#pragma once
#include "graph/routes.h"

#include <unordered_map>

#include "catalogue.h"

namespace transport {

class Graph : public graph::DirectedWeightedGraph<double> {
    using Transfer = std::pair<graph::VertexId, graph::VertexId>;

    struct Edge {
        domain::StopPtr from;
        domain::StopPtr to;
        domain::BusPtr bus;
        size_t stop_count;
        double timerange;
    };

public:
    explicit Graph(const Catalogue& db)
            : graph::DirectedWeightedGraph<double>(2*db.GetStops().size()) {
        FillStopEdges(db);
        // FillBusEdges(db);
    }

private:
    std::unordered_map<domain::StopPtr, Transfer> stop_to_transfer_;
    std::unordered_map<graph::EdgeId, Edge> id_to_edge_;

    static std::vector<Graph::Edge> CreateBusEdges(const Catalogue& db);

    void FillStopEdges(const Catalogue& db);

    // void FillBusEdges(const Catalogue& db);

};



} // namespace transport
