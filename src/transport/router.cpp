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
            Edge{.from = stop_ptr, .to = stop_ptr, .timerange = wait_time}
        });
    }
}

namespace {



} // namespace

std::vector<Graph::Edge> Graph::CreateBusEdges(const Catalogue& db) {
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
                    .timerange = 60. * distance/bus_ptr->velocity // [h -> min]
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

} // namespace transport