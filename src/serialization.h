#pragma once

#include <catalogue.pb.h>
#include <database.pb.h>
#include <domain.pb.h>
#include <geo.pb.h>
#include <graph.pb.h>

#include <sstream>

#include "request_handler.h"

namespace transport {
namespace io {

class Bufferiser {
public:
    Bufferiser(RequestHandler& request_handler)
        : request_handler_(request_handler) {
    }

    void Serialize(std::ostream& out) const;

    void Deserialize(std::istream& in);

private:
    RequestHandler& request_handler_;

    inline static svg::pb::Point Convert(const svg::Point& point) {
        svg::pb::Point converted;
        converted.set_x(point.x);
        converted.set_y(point.y);
        return converted;
    }

    inline static svg::pb::Color Convert(const svg::Color& color) {
        svg::pb::Color converted;
        std::stringstream ss;
        ss << color;
        converted.set_name(ss.str());
        return converted;
    }

    static pb::renderer::Settings Convert(const renderer::Settings& settings);

    static renderer::Settings Convert(const pb::renderer::Settings& settings);

    static graph::pb::Graph Convert(
        const graph::DirectedWeightedGraph<double>& graph
    );

    static graph::DirectedWeightedGraph<double> Convert(
        const graph::pb::Graph& graph
    );

    pb::domain::Stop Convert(const domain::Stop& stop) const;

    pb::domain::AdjacentStops Convert(
        const Catalogue::AdjacentStops adjacent_stops,
        const int distance
    ) const;

    pb::domain::Bus Convert(const domain::Bus& bus) const;

    domain::Bus Convert(const pb::domain::Bus& bus) const;
};

} // namespace io
} // namespace transport
