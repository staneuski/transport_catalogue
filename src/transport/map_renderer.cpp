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
    DrawStops(document, projector, stop_stats);
    DrawStopLabels(document, projector, stop_stats);

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
    const SphereProjector& projector,
    const domain::SetStat<domain::Route>& routes
) const {
    size_t counter = 0;
    for (const domain::Route& route : routes) {
        const std::vector<domain::StopPtr>& stops = route.ptr->stops;

        svg::Polyline polyline;
        for (auto it = stops.begin(); it != stops.end(); ++it)
            polyline.AddPoint(projector((*it)->coords));

        if (!route.ptr->is_roundtrip)
            for (auto it = stops.rbegin() + 1; it != stops.rend(); ++it)
                polyline.AddPoint(projector((*it)->coords));

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
                            const Settings::Label& label,
                            const svg::Color& color,
                            const bool is_bold) const {
    svg::Text text;

    text.SetFillColor(color)
        .SetPosition(position)
        .SetOffset(label.offset)
        .SetFontSize(label.font_size)
        .SetFontFamily("Verdana")
        .SetData(content);

    if (is_bold)
        text.SetFontWeight("bold");

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
    const SphereProjector& projector,
    const domain::SetStat<domain::Route>& routes
) const {
    bool is_bold = true;

    size_t counter = 0;
    for (const domain::Route& route : routes) {
        svg::Color color = GetRouteColor(counter++);

        if (!route.ptr->is_roundtrip
         && route.ptr->stops.front() != route.ptr->stops.back())
            DrawLabel(
                document,
                route.ptr->name,
                projector(route.ptr->stops.front()->coords),
                settings_.bus_label,
                color,
                is_bold
            );

        DrawLabel(
            document,
            route.ptr->name,
            projector(route.ptr->stops.back()->coords),
            settings_.bus_label,
            color,
            is_bold
        );
    }
}

void MapRenderer::DrawStops(
    svg::Document& document,
    const SphereProjector& projector,
    const domain::SetStat<domain::StopStat>& stop_stats
) const {
    for (const domain::StopStat& stop_stat : stop_stats) {
        svg::Circle circle;
        circle
            .SetCenter(projector(stop_stat.ptr->coords))
            .SetRadius(settings_.stop_radius)
            .SetFillColor("white");
        document.Add(circle);
    }
}

void MapRenderer::DrawStopLabels(
    svg::Document& document,
    const SphereProjector& projector,
    const domain::SetStat<domain::StopStat>& stop_stats
) const {
    for (const domain::StopStat& stop_stat : stop_stats)
        DrawLabel(
            document,
            stop_stat.ptr->name,
            projector(stop_stat.ptr->coords),
            settings_.stop_label
        );
}

} // end namespace renderer
} // end namespace transport

