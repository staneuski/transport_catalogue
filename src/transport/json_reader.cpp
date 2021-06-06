#include "json_reader.h"

namespace transport {
namespace io {

void JsonReader::ParseBuses() {
    const json::Array& base_requests = requests_.at("base_requests").AsArray();

    buses_.reserve(base_requests.size()/2u);
    for (const auto& base_request : base_requests) {
        const json::Dict& request = base_request.AsMap();

        if (request.at("type") == "Bus")
            buses_.push_back(std::make_unique<const json::Dict>(request));
        else if (request.at("type") != "Stop")
            throw std::invalid_argument(
                "unable to load base request type '" 
                + request.at("type").AsString() + "'"
            );
    }
}

void JsonReader::ParseStops() {
    const json::Array& base_requests = requests_.at("base_requests").AsArray();

    stops_.reserve(base_requests.size()/2u);
    for (const auto& base_request : base_requests) {
        const json::Dict& request = base_request.AsMap();

        if (request.at("type") == "Stop")
            stops_.push_back(std::make_unique<const json::Dict>(request));
        else if (request.at("type") != "Bus")
            throw std::invalid_argument(
                "unable to load base request type '" 
                + request.at("type").AsString() + "'"
            );
    }
}

void JsonReader::ParseStats() {
    const json::Array& stat_requests = requests_.at("stat_requests").AsArray();

    stats_.reserve(stat_requests.size());
    for (const auto& request_node : stat_requests) {
        const json::Dict& request = request_node.AsMap();

        const json::Node& type_name = request.at("type");
        if ("Bus" == type_name || "Stop" == type_name || "Map" == type_name)
            stats_.push_back(std::make_unique<const json::Dict>(request));
        else
            throw std::invalid_argument(
                "unable to load stat request type '" + type_name.AsString() + "'"
            );
    }
}

void Populate(catalogue::TransportCatalogue& db, const JsonReader& reader) {
    for (const auto& request : reader.GetStops())
        db.AddStop({
            request->at("name").AsString(),
            {request->at("latitude").AsDouble(), request->at("longitude").AsDouble()}
        });

    for (const auto& request : reader.GetStops()) {
        domain::StopPtr stop_ptr = db.SearchStop(
            request->at("name").AsString()
        );
        for (const auto& [stop_name, distance] : request->at("road_distances").AsMap())
            db.MakeAdjacent(
                stop_ptr,
                db.SearchStop(stop_name),
                distance.AsInt()
            );
    }

    for (const auto& request : reader.GetBuses()) {
        const auto& stop_names = request->at("stops").AsArray();

        std::vector<domain::StopPtr> stops;
        stops.reserve(stop_names.size());
        for (const json::Node& stop_name : stop_names)
            stops.push_back(db.SearchStop(stop_name.AsString()));

        db.AddBus({
            request->at("name").AsString(),
            stops,
            request->at("is_roundtrip").AsBool()
        });
    }
}

void Search(const RequestHandler& handler, const JsonReader& reader) {
    std::cout << "[\n";

    bool is_first = true;
    for (const auto& request : reader.GetStats()) {
        if (is_first)
            is_first = false;
        else
            std::cout << ",\n";
        std::cout << std::string(INDENT_SIZE, ' ') << '{';

        const json::Node& type_name = request->at("type");
        if ("Bus" == type_name)
            std::cout << handler.GetBusStat(type_name.AsString())
                      << ", \"request_id\": " << request->at("id") << '}';
        else if ("Stop" == type_name)
            std::cout << handler.GetStopStat(type_name.AsString())
                      << ", \"request_id\": " << request->at("id") << '}';
        else if ("Map" == type_name)
            std::cout << "\"map\": \"" << "handler.RenderMap()" << "\""
                      << ", \"request_id\": " << request->at("id") << '}';
        else
            throw std::invalid_argument(
                "unable to load stat request type '" + type_name.AsString() + "'"
            );
    }

    std::cout << "\n]" << std::endl;
}

} // end namespace io
} // end namespace transport