#pragma once

#include "catalogue.h"
#include "map_renderer.h"
#include "request_handler.h"
#include "router.h"

namespace transport {
namespace io {

class RequestHandler {
public:
    RequestHandler(Catalogue& catalogue, renderer::Settings render_settings)
        : catalogue_(catalogue)
        , renderer_(renderer::MapRenderer(render_settings))
        , router_(Router(catalogue)) {
    }

    inline void SetRendererSettings(renderer::Settings settings) {
        renderer_.SetSettings(settings);
    }

    inline Catalogue& GetCatalogue() const {
        return catalogue_;
    }

    inline renderer::Settings GetRendererSettings() const {
        return renderer_.GetSettings();
    }

    inline std::optional<domain::BusLine> GetBusStat(
        const std::string_view bus_name
    ) const {
        return catalogue_.GetBusLine(bus_name);
    }

    inline std::optional<domain::StopStat> GetStopStat(
        const std::string_view stop_name
    ) const {
        return catalogue_.GetStop(stop_name);
    }

    inline std::optional<domain::Route> GetRoute(
        const std::string_view start,
        const std::string_view finish
    ) const {
        const domain::StopPtr& start_ptr = catalogue_.SearchStop(start);
        const domain::StopPtr& finish_ptr = catalogue_.SearchStop(finish);

        return (start_ptr && finish_ptr)
               ? router_.GetRoute(start_ptr, finish_ptr)
               : std::nullopt;
    }

    inline svg::Document RenderMap() const {
        return renderer_.RenderMap(
            catalogue_.GetAllBusLines(),
            catalogue_.GetAllStopStats()
        );
    }

private:
    Catalogue& catalogue_;
    renderer::MapRenderer renderer_;
    const Router router_;
};

} // namespace io
} // namespace transport