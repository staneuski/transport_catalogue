#pragma once

#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>

#include "json/json.h"
#include "map_renderer.h"
#include "request_handler.h"
#include "transport_catalogue.h"

namespace transport {
namespace io {

static const int INDENT_SIZE = 2;

class JsonReader {
    using Request = std::unique_ptr<const json::Dict>;

public:
    JsonReader(std::istream& input)
            : requests_(json::Load(input).GetRoot().AsDict()) {
        if (requests_.find("base_requests") != requests_.end()) {
            ParseBuses();
            ParseStops();
        }
        if (requests_.find("render_settings") != requests_.end())
            ParseRenderSettings();
        if (requests_.find("stat_requests") != requests_.end())
            ParseStats();
    }

    inline const std::vector<Request>& GetBuses() const {
        return buses_;
    }

    inline const std::vector<Request>& GetStops() const {
        return stops_;
    }

    renderer::Settings GenerateMapSettings() const;

    inline const std::vector<Request>& GetStats() const {
        return stats_;
    }

private:
    json::Dict requests_;
    std::vector<Request> buses_, stops_, stats_;
    Request render_settings_;

    static svg::Color ConvertToColor(const json::Node node);

    void ParseBuses();

    void ParseStops();

    inline void ParseRenderSettings() {
        render_settings_ = std::make_unique<const json::Dict>(
            requests_.at("render_settings").AsDict()
        );
    }

    void ParseStats();
};

void Populate(catalogue::TransportCatalogue& db, const JsonReader& reader);

void Search(const RequestHandler& handler, const JsonReader& reader);

} // end namespace io
} // end namespace transport