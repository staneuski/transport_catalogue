#pragma once

#include <vector>

#include "svg/svg.h"

namespace transport {
namespace renderer {

struct Settings {
    struct Label { uint32_t font_size; svg::Point offset; };
    struct Underlayer { double width; svg::Color color; };

    std::pair<double, double> map_sizes;
    double padding;
    double line_width;
    Label bus_label, stop_label;
    Underlayer underlayer;
    std::vector<svg::Color> palette;
};

class MapRenderer {
public:
    MapRenderer(Settings settings) : settings_(std::move(settings)) {}

    inline svg::Document RenderMap() const {
        svg::Document document_;
        return document_;
    }

private:
    Settings settings_;
};

} // end namespace renderer
} // end namespace transport