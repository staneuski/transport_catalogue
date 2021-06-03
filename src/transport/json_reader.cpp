#include "json_reader.h"

namespace transport {
namespace io {

Requests LoadRequests() {
    json::Dict type_to_requests = std::move(
        json::Load(std::cin).GetRoot().AsMap()
    );

    json::Array base_requests = std::move(
        type_to_requests.at("base_requests").AsArray()
    );
    json::Array stat_requests = std::move(
        type_to_requests.at("stat_requests").AsArray()
    );

    Requests requests;

    requests.buses.reserve(base_requests.size());
    for (const auto& request_node : base_requests) {
        const json::Dict& request = request_node.AsMap();

        if (request.at("type") == "Bus")
            requests.buses.push_back({
                request.at("name").AsString(),
                ConvertNodes(request.at("stops").AsArray()),
                request.at("is_roundtrip").AsBool()
            });
        else if (request.at("type") != "Stop")
            throw std::invalid_argument(
                "unable to load base request type '" 
                + request.at("type").AsString() + "'"
            );
    }

    requests.stops.reserve(base_requests.size());
    for (const auto& request_node : base_requests) {
        const json::Dict& request = request_node.AsMap();

        if (request.at("type") == "Stop")
            requests.stops.push_back({
                request.at("name").AsString(),
                request.at("latitude").AsDouble(),
                request.at("longitude").AsDouble(),
                ConvertNodes(request.at("road_distances").AsMap())
            });
    }

    requests.stats.reserve(stat_requests.size());
    for (const auto& request_node : stat_requests) {
        const json::Dict& request = request_node.AsMap();

        StatRequest stat_request;
        stat_request.ip = request.at("id").AsInt();
        stat_request.name = request.at("name").AsString();
        if (request.at("type") == "Bus")
            stat_request.type = RequestType::BUS;
        else if (request.at("type") == "Stop")
            stat_request.type = RequestType::STOP;
        else
            throw std::invalid_argument(
                "unable to load stat request type '"
                + request.at("type").AsString() + "'"
            );

        requests.stats.push_back(stat_request);
    }

    return requests;
}

} // end namespace io
} // end namespace transport