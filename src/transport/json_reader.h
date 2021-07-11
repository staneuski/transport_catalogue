#pragma once

#include <iostream>
#include <memory>
#include <optional>
#include <sstream>
#include <stdexcept>

#include "json/json_builder.h"
#include "map_renderer.h"
#include "request_handler.h"
#include "transport_catalogue.h"

namespace transport {
namespace io {

class JsonReader {
    using Request = std::unique_ptr<const json::Dict>;

    enum class BaseType { BUS, STOP, };

    struct Settings {
        Request render;
        Request routing;
    };

public:
    JsonReader(std::istream& input)
            : requests_(json::Load(input).GetRoot().AsDict()) {
        if (requests_.find("base_requests") != requests_.end()) {
            ParseBases(BaseType::BUS);
            ParseBases(BaseType::STOP);
        }

        if (requests_.find("render_settings") != requests_.end())
            ParseSettings("render_settings");

        if (requests_.find("routing_settings") != requests_.end())
            ParseSettings("routing_settings");

        if (requests_.find("stat_requests") != requests_.end())
            ParseStats();
    }

    renderer::Settings GenerateMapSettings() const;

    inline const std::vector<Request>& GetBuses() const {
        return buses_;
    }

    inline const std::vector<Request>& GetStops() const {
        return stops_;
    }

    inline const std::vector<Request>& GetStats() const {
        return stats_;
    }

private:
    json::Dict requests_;
    std::vector<Request> buses_;
    std::vector<Request> stops_;
    std::vector<Request> stats_;
    Settings settings_;

    static svg::Color ConvertToColor(const json::Node node);

    static std::string ConvertRequestType(const BaseType type);

    void ParseBases(const BaseType type);

    void ParseSettings(const std::string& setting_key);

    void ParseStats();
};

void Populate(catalogue::TransportCatalogue& db, const JsonReader& reader);

void Search(const RequestHandler& handler, const JsonReader& reader);

} // end namespace io
} // end namespace transport