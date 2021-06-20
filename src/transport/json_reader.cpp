#include "json_reader.h"

namespace transport {
namespace io {

std::string ConvertRequestType(const RequestType request_e) {
    switch (request_e) {
    case RequestType::BUS:
        return "Bus";
    case RequestType::STOP:
        return "Stop";
    default:
        throw std::invalid_argument("BaseRequest enumiration option");
    }
}

JsonReader::JsonReader(std::istream& input) 
        : requests_(json::Load(input).GetRoot().AsMap()) {
    if (requests_.find("base_requests") != requests_.end()) {
        ParseBases(RequestType::BUS);
        ParseBases(RequestType::STOP);
    }
    if (requests_.find("render_settings") != requests_.end())
        ParseRenderSettings();
    if (requests_.find("stat_requests") != requests_.end())
        ParseStats();
}

void JsonReader::ParseBases(const RequestType type_e) {
    const json::Array& base_requests = requests_.at("base_requests").AsArray();

    if (RequestType::BUS == type_e && !buses_.capacity())
        buses_.reserve(base_requests.size()/2u);
    if (RequestType::STOP == type_e && !stops_.capacity())
        stops_.reserve(base_requests.size()/2u);

    const std::string type_name = ConvertRequestType(type_e);
    for (const auto& base_request : base_requests)
        AppendBase(base_request.AsMap(), type_name);
}

void JsonReader::ParseStats() {
    const json::Array& stat_requests = requests_.at("stat_requests").AsArray();

    stats_.reserve(stat_requests.size());
    for (const auto& request_node : stat_requests) {
        const json::Dict& request = request_node.AsMap();

        const json::Node& type_node = request.at("type");
        if (type_node == "Bus" || type_node == "Stop" || type_node == "Map")
            stats_.push_back(std::make_unique<const json::Dict>(request));
        else
            throw std::invalid_argument(
                "unable to load stat request type '" + type_node.AsString() + "'"
            );
    }
}

void JsonReader::AppendBase(const json::Dict& request,
                            const std::string& type_name) {
    const json::Node& type_node = request.at("type");
    if (type_node != "Stop" && type_node != "Bus" && type_node != "Map")
        throw std::invalid_argument(
            "unable to load request with type '" + type_node.AsString() + "'"
        );
    else if (type_name == "Bus" && type_name == type_node)
        buses_.push_back(std::make_unique<const json::Dict>(request));
    else if (type_name == "Stop" && type_name == type_node)
        stops_.push_back(std::make_unique<const json::Dict>(request));
}

renderer::Settings JsonReader::GenerateMapSettings() const {
    renderer::Settings settings;

    const auto fill = [&](renderer::Settings::Label& label,
                         const std::string& label_key) {
        const json::Array& arr = render_settings_->at(label_key + "_offset").AsArray();
        label = {
            render_settings_->at(label_key + "_font_size").AsInt(),
            {arr.front().AsDouble(), arr.back().AsDouble()}
        };
    };

    if (requests_.find("render_settings") == requests_.end())
        return settings;

    settings.map_sizes = {
        render_settings_->at("width").AsDouble(),
        render_settings_->at("height").AsDouble()
    };
    settings.padding = render_settings_->at("padding").AsDouble();
    settings.stop_radius = render_settings_->at("stop_radius").AsDouble();
    settings.line_width = render_settings_->at("line_width").AsDouble();

    fill(settings.bus_label, "bus_label");
    fill(settings.stop_label, "stop_label");

    settings.underlayer = {
        render_settings_->at("underlayer_width").AsDouble(),
        ConvertToColor(render_settings_->at("underlayer_color")),
    };

    const json::Array& color_palette = render_settings_->at("color_palette").AsArray();
    settings.palette.reserve(color_palette.size());
    for (const json::Node& color : color_palette)
        settings.palette.push_back(ConvertToColor(color));

    return settings;
}

svg::Color JsonReader::ConvertToColor(const json::Node node) {
    svg::Color color;

    if (node.IsString())
        color = node.AsString();
    else if (node.IsArray() && node.AsArray().size() == 3u)
        color = svg::Rgb(
            node.AsArray().at(0).AsInt(),
            node.AsArray().at(1).AsInt(),
            node.AsArray().at(2).AsInt()
        );
    else if (node.IsArray() && node.AsArray().size() == 4u)
        color = svg::Rgba(
            node.AsArray().at(0).AsInt(),
            node.AsArray().at(1).AsInt(),
            node.AsArray().at(2).AsInt(),
            node.AsArray().at(3).AsDouble()
        );
    else
        throw std::invalid_argument("unable to colvert node value to color");

    return color;
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

        if ("Bus" == request->at("type")) {
            std::cout << handler.GetBusStat(request->at("name").AsString())
                      << ", \"request_id\": " << request->at("id") << '}';
        } else if ("Stop" == request->at("type")) {
            std::cout << handler.GetStopStat(request->at("name").AsString())
                      << ", \"request_id\": " << request->at("id") << '}';
        } else if ("Map" == request->at("type")) {
            std::ostringstream out;
            handler.RenderMap().Render(out);

            std::cout << "\"map\": " << json::Node(out.str())
                      << ", \"request_id\": " << request->at("id") << '}';
        } else {
            throw std::invalid_argument(
                "unable to load stat request type '"
                + request->at("type").AsString() + "'"
            );
        }
    }

    std::cout << "\n]" << std::endl;
}

} // end namespace io
} // end namespace transport