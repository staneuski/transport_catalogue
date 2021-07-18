#include "router.h"

#include <iterator>
#include <vector>

namespace transport {

using graph::EdgeId, graph::VertexId;

void Router::FillStopEdges(const Catalogue& db) {
    graph::VertexId vertex_id = {};
    for (const auto& [_, stop_ptr] : db.GetStopsHolder()) {
        stop_to_transfer_.emplace(stop_ptr, Transfer{vertex_id, vertex_id++});
        ++vertex_id;
    }

    for (const auto& [stop_ptr, transfer] : stop_to_transfer_) {
        const double wait_time = stop_ptr->wait_time;
        id_to_edge_.emplace(
            graph_.AddEdge({transfer.second, transfer.first, wait_time}),
            domain::Edge{.from = stop_ptr, .to = stop_ptr, .timedelta = wait_time}
        );
    }
}

std::vector<domain::Edge> Router::CreateEdgesFromBusLines(const Catalogue& db) {
    const auto& stops_to_distance = db.GetDistances();
    const auto& push_back_busline_edges = [&](auto first, auto last,
                                              std::vector<domain::Edge>& edges,
                                              const domain::BusPtr& bus_ptr) {
        using AdjacentStops = std::pair<domain::StopPtr, domain::StopPtr>;

        for (auto from = first; from != last; ++from) {
            int distance = 0;
            for (auto to = std::next(from); to != last; ++to) {
                if (*to == *from)
                    continue;

                const domain::StopPtr& prev = *std::prev(to);
                distance += stops_to_distance.at(
                    stops_to_distance.find({prev, *to}) != stops_to_distance.end()
                    ? AdjacentStops(prev, *to)
                    : AdjacentStops(*to, prev)
                );

                edges.push_back(domain::Edge{
                    .from = *from,
                    .to = *to,
                    .bus = bus_ptr,
                    .stop_count = static_cast<uint8_t>(std::distance(from, to)),
                    .timedelta = 60 * distance*1e-3/bus_ptr->velocity // [h]->[min]
                });
            }
        }
    };

    std::vector<domain::Edge> edges;
    for (const auto& [_, bus_ptr] : db.GetBusesHolder()) {
        push_back_busline_edges(
            begin(bus_ptr->stops), end(bus_ptr->stops),
            edges,
            bus_ptr
        );

        if (!bus_ptr->is_roundtrip)
            push_back_busline_edges(
                rbegin(bus_ptr->stops), rend(bus_ptr->stops),
                edges,
                bus_ptr
            );
    }

    return edges;
}

void Router::FillBusEdges(const Catalogue& db) {
    std::unordered_map<graph::VertexId, std::unordered_map<graph::VertexId, domain::Edge>> vertex_to_edges;
    for (domain::Edge& edge : CreateEdgesFromBusLines(db)) {
        const graph::VertexId from = stop_to_transfer_.at(edge.from).first;
        const graph::VertexId to = stop_to_transfer_.at(edge.to).second;

        if (vertex_to_edges.count(from) && vertex_to_edges.at(from).count(to)) {
            if (vertex_to_edges.at(from).at(to).timedelta > edge.timedelta)
                vertex_to_edges.at(from).at(to) = std::move(edge);
        } else {
            vertex_to_edges[from].emplace(to, std::move(edge));
        }
    }

    for (auto& [from, vertex_to_edge] : vertex_to_edges)
        for (auto& [to, edge] : vertex_to_edge)
            id_to_edge_.emplace(
                graph_.AddEdge({from, to, edge.timedelta}),
                std::move(edge)
            );
}

std::vector<domain::Edge> Router::GetEdgesFromIds(
    std::vector<graph::EdgeId> edge_ids
) const {
    std::vector<domain::Edge> edges;
    edges.reserve(edge_ids.size());
    for (const graph::EdgeId id : edge_ids)
        edges.push_back(id_to_edge_.at(id));
    return edges;
}

std::optional<domain::Route> Router::GetRoute(
    const domain::StopPtr& start,
    const domain::StopPtr& finish
) const {
    const auto& route = router_->BuildRoute(
        stop_to_transfer_.at(start).second,
        stop_to_transfer_.at(finish).second
    );

    if (!route)
        return std::nullopt;

    return domain::Route{GetEdgesFromIds(route->edges), route->weight};
}

} // namespace transport