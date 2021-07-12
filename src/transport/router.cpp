#include "router.h"

namespace transport {

using graph::EdgeId, graph::VertexId;



void Graph::FillStopEdges(const std::vector<domain::StopPtr>& stops) {
    graph::VertexId vertex_id = {};
    for (const domain::StopPtr& stop_ptr : stops)
        stop_to_transfer_.insert({stop_ptr, {vertex_id++, vertex_id++}});

    for (const auto& [stop_ptr, vertex_id] : stop_to_transfer_) {
        const double wait_time = stop_ptr->wait_time;
        edges_.insert({
            AddEdge({vertex_id.second, vertex_id.first, wait_time}),
            Edge{.from = stop_ptr, .to = stop_ptr, .timerange = wait_time}
        });
    }
}

namespace {



} // namespace

void Graph::FillBusEdges(const std::vector<domain::BusPtr>& buses,
                         const Distances& stops_to_distance) {
    (void) buses;
    (void) stops_to_distance;
}

} // namespace transport