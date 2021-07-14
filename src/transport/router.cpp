#include "router.h"

#include <iterator>
#include <vector>

namespace transport {

using graph::EdgeId, graph::VertexId;



void Graph::FillStopEdges(const Catalogue& db) {
    graph::VertexId vertex_id = {};
    for (const domain::StopPtr& stop_ptr : db.GetStops())
        stop_to_transfer_.insert({stop_ptr, {vertex_id++, vertex_id++}});

    for (const auto& [stop_ptr, vertex_id] : stop_to_transfer_) {
        const double wait_time = stop_ptr->wait_time;
        id_to_edge_.insert({
            AddEdge({vertex_id.second, vertex_id.first, wait_time}),
            Edge{.from = stop_ptr, .to = stop_ptr, .timedelta = wait_time}
        });
    }
}

std::vector<Graph::Edge> Graph::CreateEdgesFromBusLines(const Catalogue& db) {
    const auto& stops_to_distance = db.GetDistances();
    const auto& convert_busline_to_edges = [&](auto first, auto last,
                                               const domain::BusPtr& bus_ptr) {
        using AdjacentStops = std::pair<domain::StopPtr, domain::StopPtr>;

        std::vector<Graph::Edge> edges;

        size_t capacity = 0;
        for (uint i = 1; i + 1 < std::distance(first, last); ++i)
            capacity += i;
        edges.reserve(capacity);

        for (auto it = first; it != last; ++it) {
            int distance = 0;
            size_t count = 0;
            for (auto inner_it = std::next(it); inner_it != last; ++inner_it) {
                if (*it == *inner_it)
                    continue;

                const domain::StopPtr prev_stop = *std::prev(inner_it);
                const domain::StopPtr stop = *inner_it;

                distance += stops_to_distance.at(
                    stops_to_distance.find({prev_stop, stop}) != stops_to_distance.end()
                    ? AdjacentStops(prev_stop, stop)
                    : AdjacentStops(stop, prev_stop)
                );

                edges.push_back(Graph::Edge{
                    .from = prev_stop,
                    .to = stop,
                    .bus = bus_ptr,
                    .stop_count = count++,
                    .timedelta = 60. * distance/bus_ptr->velocity // [h -> min]
                });
            }
        }

        return edges;
    };

    std::vector<Graph::Edge> edges;
    for (const domain::BusPtr& bus : db.GetBuses()) {
        edges = convert_busline_to_edges(begin(bus->stops), end(bus->stops), bus);

        if (bus->is_roundtrip) {
            std::vector<Graph::Edge> redges = convert_busline_to_edges(
                rbegin(bus->stops), rend(bus->stops),
                bus
            );
            edges.insert(edges.end(), std::make_move_iterator(redges.begin()), 
                                      std::make_move_iterator(redges.end()));
        }
    }

    return edges;
}

void Graph::FillBusEdges(const Catalogue& db) {
    std::unordered_map<graph::VertexId, std::unordered_map<graph::VertexId, Graph::Edge>> vertex_to_edges;
    for (Graph::Edge& edge : CreateEdgesFromBusLines(db)) {
        const graph::VertexId from = stop_to_transfer_.at(edge.from).first;
        const graph::VertexId to = stop_to_transfer_.at(edge.to).second;

        if (vertex_to_edges.find(from) != vertex_to_edges.end()
         && vertex_to_edges.at(from).find(to) != vertex_to_edges.at(from).end()) {
            if (edge.timedelta < vertex_to_edges.at(from).at(to).timedelta)
                vertex_to_edges.at(from).at(to) = std::move(edge);
        } else {
            vertex_to_edges[from].emplace(to, std::move(edge));
        }
    }

    for (auto& [from, vertex_to_edge] : vertex_to_edges)
        for (auto& [to, edge] : vertex_to_edge)
            id_to_edge_.emplace(
                AddEdge({from, to, edge.timedelta}),
                std::move(edge)
            );
}

} // namespace transport