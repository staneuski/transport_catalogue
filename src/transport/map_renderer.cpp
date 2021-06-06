#include "map_renderer.h"

namespace transport {
namespace renderer {

svg::Document MapRenderer::RenderMap(
    const domain::SetStat<domain::Route>&,
    const domain::SetStat<domain::StopStat>& stop_stats
) const {
    svg::Document document_;

    const std::vector<geo::Coordinates> coordinates = GetCoordinates(stop_stats);
    SphereProjector projector(
        coordinates.begin(), coordinates.end(),
        settings_.map_sizes.first, settings_.map_sizes.second,
        settings_.padding
    );
    (void) projector;

    return document_;
}

std::vector<geo::Coordinates> MapRenderer::GetCoordinates(
    const domain::SetStat<domain::StopStat>& stop_stats
) const {
    std::vector<geo::Coordinates> coordinates;
    coordinates.reserve(stop_stats.size());
    for (const auto& stop_stat : stop_stats)
        if (!stop_stat.unique_buses.empty())
            coordinates.push_back(stop_stat.ptr->coords);
    return coordinates;
}

} // end namespace renderer
} // end namespace transport
