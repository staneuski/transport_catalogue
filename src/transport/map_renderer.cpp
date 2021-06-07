#include "map_renderer.h"

namespace transport {
namespace renderer {

svg::Document MapRenderer::RenderMap(
    const domain::SetStat<domain::Route>& routes,
    const domain::SetStat<domain::StopStat>& stop_stats
) const {
    svg::Document document;

    const std::vector<geo::Coordinates> coordinates = GetCoordinates(stop_stats);
    SphereProjector projector(
        coordinates.begin(), coordinates.end(),
        settings_.map_sizes.first, settings_.map_sizes.second,
        settings_.padding
    );

    DrawRouteLines(document, projector, routes);
    DrawRouteLabels(document, projector, routes);

    return document;
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

void MapRenderer::DrawRouteLines(
    svg::Document& document,
    SphereProjector& projector,
    const domain::SetStat<domain::Route>& routes
) const {
    size_t counter = 0;
    for (const domain::Route& route : routes) {
        svg::Polyline polyline;
        for (const domain::StopPtr& stop_ptr : route.ptr->stops)
            polyline.AddPoint(projector(stop_ptr->coords));

        document.Add(
            polyline
                .SetStrokeColor(GetRouteColor(counter++))
                .SetFillColor(svg::Color())
                .SetStrokeWidth(settings_.line_width)
                .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND)
        );
    }
}

void MapRenderer::DrawLabel(svg::Document& document,
                            const std::string& content,
                            const svg::Point& position,
                            const svg::Color& color,
                            const Settings::Label& label) const {
    svg::Text text;
    text.SetFillColor(color)
        .SetPosition(position)
        .SetOffset(label.offset)
        .SetFontSize(label.font_size)
        .SetFontFamily("Verdana")
        .SetFontWeight("bold")
        .SetData(content);

    svg::Text back = text;
    back.SetFillColor(settings_.underlayer.color)
        .SetStrokeWidth(settings_.underlayer.width)
        .SetStrokeColor(settings_.underlayer.color)
        .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
        .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

    document.Add(back);
    document.Add(text);
}

void MapRenderer::DrawRouteLabels(
    svg::Document& document,
    SphereProjector& projector,
    const domain::SetStat<domain::Route>& routes
) const {
    size_t counter = 0;
    for (const domain::Route& route : routes) {
        DrawLabel(
            document,
            route.ptr->name,
            projector(route.ptr->stops.back()->coords),
            GetRouteColor(counter),
            settings_.bus_label
        );

        if (!route.ptr->is_roundtrip)
            DrawLabel(
                document,
                route.ptr->name,
                projector(route.ptr->stops.front()->coords),
                GetRouteColor(counter),
                settings_.bus_label
            );

        ++counter;
    }
}

} // end namespace renderer
} // end namespace transport
