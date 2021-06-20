#pragma once

#include <algorithm>
#include <cmath>
#include <vector>

#include "geo/geo.h"
#include "svg/svg.h"

#include "domain.h"

namespace transport {

static const double EPSILON = 1e-6;
inline bool IsZero(double value) {
    return std::abs(value) < EPSILON;
}

namespace renderer {

class SphereProjector {
public:
    template <typename PointInputIt>
    SphereProjector(
        PointInputIt points_begin,
        PointInputIt points_end,
        double max_width,
        double max_height,
        double padding
    ) : padding_(padding) {
        if (points_begin == points_end)
            return;

        const auto [left_it, right_it] = std::minmax_element(
            points_begin,
            points_end,
            [](auto lhs, auto rhs) { return lhs.lng < rhs.lng; }
        );
        min_lon_ = left_it->lng;
        const double max_lon = right_it->lng;

        const auto [bottom_it, top_it] = std::minmax_element(
            points_begin,
            points_end,
            [](auto lhs, auto rhs) { return lhs.lat < rhs.lat; }
        );
        const double min_lat = bottom_it->lat;
        max_lat_ = top_it->lat;

        std::optional<double> width_zoom;
        if (!IsZero(max_lon - min_lon_))
            width_zoom = (max_width - 2*padding)/(max_lon - min_lon_);

        std::optional<double> height_zoom;
        if (!IsZero(max_lat_ - min_lat))
            height_zoom = (max_height - 2*padding)/(max_lat_ - min_lat);

        if (width_zoom && height_zoom)
            zoom_coeff_ = std::min(*width_zoom, *height_zoom);
        else if (width_zoom)
            zoom_coeff_ = *width_zoom;
        else if (height_zoom)
            zoom_coeff_ = *height_zoom;
    }

    inline svg::Point operator()(const geo::Coordinates& coords) const {
        return {
            (coords.lng - min_lon_)*zoom_coeff_ + padding_,
            (max_lat_ - coords.lat)*zoom_coeff_ + padding_
        };
    }

private:
    double padding_;
    double min_lon_ = 0;
    double max_lat_ = 0;
    double zoom_coeff_ = 0;
};

struct Settings {
    struct Label { int font_size; svg::Point offset; };
    struct Underlayer { double width; svg::Color color; };

    std::pair<double, double> map_sizes;
    double padding;
    double line_width;
    double stop_radius;
    Label bus_label, stop_label;
    Underlayer underlayer;
    std::vector<svg::Color> palette;
};

class MapRenderer {
public:
    MapRenderer(Settings settings) : settings_(std::move(settings)) {}

    svg::Document RenderMap(
        const domain::SetStat<domain::Route>& routes,
        const domain::SetStat<domain::StopStat>& stop_stats
    ) const;

private:
    Settings settings_;

    inline svg::Color GetRouteColor(const size_t route_counter) const {
        return settings_.palette.at(route_counter % settings_.palette.size());
    }

    std::vector<geo::Coordinates> GetCoordinates(
        const domain::SetStat<domain::StopStat>& stops
    ) const;

    void DrawRouteLines(svg::Document& document,
                        const SphereProjector& projector,
                        const domain::SetStat<domain::Route>& routes) const;

    void DrawLabel(svg::Document& document,
                   const std::string& content,
                   const svg::Point& position,
                   const Settings::Label& label,
                   const svg::Color& color = svg::Color("black"),
                   const bool is_bold = false) const;

    void DrawRouteLabels(svg::Document& document,
                         const SphereProjector& projector,
                         const domain::SetStat<domain::Route>& routes) const;

    void DrawStops(svg::Document& document,
                   const SphereProjector& projector,
                   const domain::SetStat<domain::StopStat>& stop_stats) const;

    void DrawStopLabels(
        svg::Document& document,
        const SphereProjector& projector,
        const domain::SetStat<domain::StopStat>& stop_stats
    ) const;
};

} // end namespace renderer
} // end namespace transport