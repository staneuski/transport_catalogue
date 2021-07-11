#pragma once

#include "map_renderer.h"
#include "request_handler.h"
#include "transport_catalogue.h"

namespace transport {
namespace io {

class RequestHandler {
public:
    RequestHandler(const catalogue::TransportCatalogue& transport_catalogue,
                   const renderer::MapRenderer& renderer)
            : db_(transport_catalogue)
            , renderer_(renderer) {
    }

    inline std::optional<domain::BusLine> GetBusStat(
        const std::string_view& bus_name
    ) const {
        return db_.GetBusLine(bus_name);
    }

    inline std::optional<domain::StopStat> GetStopStat(
        const std::string_view& stop_name
    ) const {
        return db_.GetStop(stop_name);
    }

    inline svg::Document RenderMap() const {
        return renderer_.RenderMap(db_.GetAllBusLines(), db_.GetAllStopStats());
    }

private:
    const catalogue::TransportCatalogue& db_;
    const renderer::MapRenderer& renderer_;
};

} // end namespace io
} // end namespace transport