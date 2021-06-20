#pragma once

#include <iostream>
#include <memory>
#include <stdexcept>

#include "json/json.h"
#include "map_renderer.h"
#include "request_handler.h"
#include "transport_catalogue.h"

namespace transport {
namespace io {

static const int INDENT_SIZE = 2;

enum class RequestType { BUS, STOP, };

std::string ConvertRequestType(const RequestType request_t);

class JsonReader {
    using Request = std::unique_ptr<const json::Dict>;

public:
    JsonReader(std::istream& input);

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
    std::vector<Request> buses_;
    std::vector<Request> stops_;
    std::vector<Request> stats_;
    Request render_settings_;

    static svg::Color ConvertToColor(const json::Node node);

    inline void ParseRenderSettings() {
        render_settings_ = std::make_unique<const json::Dict>(
            requests_.at("render_settings").AsMap()
        );
    }

    void ParseBases(const RequestType type);

    void ParseStats();

    void AppendBase(const json::Dict& request, const std::string& type_name);
};

void Populate(catalogue::TransportCatalogue& db, const JsonReader& reader);

void Search(const RequestHandler& handler, const JsonReader& reader);

} // end namespace io
} // end namespace transport