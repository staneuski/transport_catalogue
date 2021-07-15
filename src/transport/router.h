#pragma once
#include "graph/routes.h"

#include <memory>
#include <optional>
#include <unordered_map>

#include "catalogue.h"

namespace transport {

// ---------- Graph -------------------

class Graph : public graph::DirectedWeightedGraph<double> {
    using Transfer = std::pair<graph::VertexId, graph::VertexId>;

public:
    explicit Graph(const Catalogue& db)
            : graph::DirectedWeightedGraph<double>(2*db.GetStops().size()) {
        FillStopEdges(db);
        FillBusEdges(db);
    }

    inline const Transfer& GetTransfer(const domain::StopPtr stop) const {
        return stop_to_transfer_.at(stop);
    }

    inline domain::EdgePtr GetEdgePtr(const graph::EdgeId id) const {
        return std::make_shared<domain::Edge>(id_to_edge_.at(id));
    }

private:
    std::unordered_map<domain::StopPtr, Transfer> stop_to_transfer_;
    std::unordered_map<graph::EdgeId, domain::Edge> id_to_edge_;

    static std::vector<domain::Edge> CreateEdgesFromBusLines(const Catalogue& db);

    void FillStopEdges(const Catalogue& db);

    void FillBusEdges(const Catalogue& db);
};

// ---------- Router ------------------

class Router : graph::Router<double> {
    struct Route {
        std::vector<domain::EdgePtr> edges;
        double timedelta;
    };

public:
    Router(const transport::Graph& graph)
            : graph::Router<double>(graph)
            , graph_(graph) {
    }

    std::optional<Route> GetRoute(const domain::StopPtr start,
                                  const domain::StopPtr finish) const;

private:
    const transport::Graph& graph_;

    std::vector<domain::EdgePtr> ConvertToPtrs(
        std::vector<graph::EdgeId> edge_ids
    ) const;
};

} // namespace transport