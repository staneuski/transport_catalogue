#pragma once

#include "catalogue.h"
#include "map_renderer.h"
#include "request_handler.h"
#include "router.h"

namespace transport {
namespace io {

class RequestHandler {
public:
    RequestHandler(const Catalogue& db,
                   const renderer::MapRenderer& renderer)
            : db_(db)
            , renderer_(renderer)
            , router_(Router(db)) {
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

    inline std::optional<domain::Route> GetRoute(
        const std::string_view& start,
        const std::string_view& finish
    ) const {
        const domain::StopPtr& start_ptr = db_.SearchStop(start);
        const domain::StopPtr& finish_ptr = db_.SearchStop(finish);

        if (!start_ptr || !finish_ptr)
            return std::nullopt;

        return router_.GetRoute(start_ptr, finish_ptr);
    }

    inline svg::Document RenderMap() const {
        return renderer_.RenderMap(db_.GetAllBusLines(), db_.GetAllStopStats());
    }

private:
    const Catalogue& db_;
    const renderer::MapRenderer& renderer_;
    const Router router_;
};

} // namespace io
} // namespace transport