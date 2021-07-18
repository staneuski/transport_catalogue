#pragma once
#include "graph/router.h"

#include <memory>
#include <optional>
#include <unordered_map>

#include "catalogue.h"

namespace transport {

class Router {
    using Transfer = std::pair<graph::VertexId, graph::VertexId>;

public:
    explicit Router(const Catalogue& db) : graph_(2*db.GetStopCount()) {
        FillStopEdges(db);
        FillBusEdges(db);
        router_ = std::make_unique<graph::Router<double>>(
            graph::Router<double>(graph_)
        );
    }

    std::optional<domain::Route> GetRoute(const domain::StopPtr& start,
                                          const domain::StopPtr& finish) const;

private:
    graph::DirectedWeightedGraph<double> graph_;
    std::unique_ptr<graph::Router<double>> router_;
    std::unordered_map<domain::StopPtr, Transfer> stop_to_transfer_;
    std::unordered_map<graph::EdgeId, domain::Edge> id_to_edge_;

    static std::vector<domain::Edge> CreateEdgesFromBusLines(
        const Catalogue& db
    );

    std::vector<domain::Edge> GetEdgesFromIds(
        std::vector<graph::EdgeId> edge_ids
    ) const;

    void FillStopEdges(const Catalogue& db);

    void FillBusEdges(const Catalogue& db);
};

} // namespace transport