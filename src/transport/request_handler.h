#pragma once

#include "transport_catalogue.h"

namespace transport {
namespace renderer {

class MapRenderer;

} // end namespace renderer
} // end namespace transport

namespace transport {
namespace io {

class RequestHandler {
public:
    RequestHandler(const TransportCatalogue& transport_catalogue)
            : db_(transport_catalogue) {}

    // RequestHandler(const TransportCatalogue& transport_catalogue,
    //                const renderer::MapRenderer& renderer)
    //         : db_(transport_catalogue)
    //         , renderer_(renderer) {}

    inline domain::Route GetBusStat(const std::string_view& bus_name) const {
        return db_.GetRoute(bus_name);
    }

    inline domain::StopStat GetStopStat(const std::string_view& stop_name) const {
        return db_.GetStop(stop_name);
    }

    // svg::Document RenderMap() const;

private:
    const TransportCatalogue& db_;
    // const renderer::MapRenderer& renderer_;
};

} // end namespace io
} // end namespace transport