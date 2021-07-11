#include "json_reader.h"

namespace transport {
namespace io {

renderer::Settings JsonReader::GenerateMapSettings() const {
    renderer::Settings settings;

    const auto fill = [&](renderer::Settings::Label& label,
                         const std::string& label_key) {
        const json::Array& arr = settings_.render->at(label_key + "_offset").AsArray();
        label = {
            settings_.render->at(label_key + "_font_size").AsInt(),
            {arr.front().AsDouble(), arr.back().AsDouble()}
        };
    };

    if (requests_.find("render_settings") == requests_.end())
        return settings;

    settings.map_sizes = {
        settings_.render->at("width").AsDouble(),
        settings_.render->at("height").AsDouble()
    };
    settings.padding = settings_.render->at("padding").AsDouble();
    settings.stop_radius = settings_.render->at("stop_radius").AsDouble();
    settings.line_width = settings_.render->at("line_width").AsDouble();

    fill(settings.bus_label, "bus_label");
    fill(settings.stop_label, "stop_label");

    settings.underlayer = {
        settings_.render->at("underlayer_width").AsDouble(),
        ConvertToColor(settings_.render->at("underlayer_color")),
    };

    const json::Array& color_palette = settings_.render->at("color_palette").AsArray();
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
        throw std::invalid_argument("unable to convert node value to color");

    return color;
}

std::string JsonReader::ConvertRequestType(const JsonReader::BaseType type) {
    switch (type) {
    case JsonReader::BaseType::BUS:
        return "Bus";
    case JsonReader::BaseType::STOP:
        return "Stop";
    default:
        throw std::invalid_argument("transport::io::BaseType: enum class");
    }
}

void ThrowInvalidRequest(const std::string& id, const std::string& type) {
    throw std::invalid_argument(
        "unable to load request " + id + " (type='" + type + "')"
    );
}

void JsonReader::ParseBases(const BaseType type) {
    const std::string type_s = ConvertRequestType(type);

    std::vector<Request>* container = nullptr;
    if (type == BaseType::BUS)
        container = &buses_;
    else if (type == BaseType::STOP)
        container = &stops_;

    const json::Array& base_requests = requests_.at("base_requests").AsArray();
    container->reserve(base_requests.size()/2u);
    for (const auto& base_request : base_requests) {
        const json::Dict& request = base_request.AsDict();
        const json::Node& type_value = request.at("type");

        if (type_value == type_s)
            container->push_back(std::make_unique<const json::Dict>(request));
        else if (type_value != "Bus" && type_value != "Stop" && type_value != "Map")
            ThrowInvalidRequest(
                request.at("id").AsString(),
                type_value.AsString()
            );
    }
}

void JsonReader::ParseSettings(const std::string& setting_key) {
    Request settings = std::make_unique<const json::Dict>(
        requests_.at(setting_key).AsDict()
    );
    if (setting_key == "render_settings")
        settings_.render = std::move(settings);
    else if (setting_key == "routing_settings")
        settings_.routing = std::move(settings);
}

void JsonReader::ParseStats() {
    const json::Array& stat_requests = requests_.at("stat_requests").AsArray();

    stats_.reserve(stat_requests.size());
    for (const auto& request_node : stat_requests) {
        const json::Dict& request = request_node.AsDict();
        const json::Node& type_value = request.at("type");

        if ("Bus" == type_value || "Stop" == type_value || "Map" == type_value)
            stats_.push_back(std::make_unique<const json::Dict>(request));
        else
            ThrowInvalidRequest(
                request.at("id").AsString(),
                type_value.AsString()
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
        for (const auto& [stop_name, distance] : request->at("road_distances").AsDict())
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

    if (const auto& handler = reader.GetRoutingSettings(); handler)
        db.SetTiming({
            handler->at("bus_velocity").AsInt(),
            handler->at("bus_wait_time").AsInt()
        });
}

json::Node ConstructNotFoundRequest(const int id) {
    return json::Builder{}.StartDict()
        .Key("error_message").Value("not found")
        .Key("request_id").Value(id)
    .EndDict()
    .Build();
}

json::Node ConstructRouteRequest(const int id,
                                 const std::optional<domain::Route>& route) {
    if (route)
        return json::Builder{}.StartDict()
            .Key("curvature").Value(route->curvature)
            .Key("request_id").Value(id)
            .Key("route_length").Value(route->length)
            .Key("stop_count").Value(static_cast<int>(route->stops_count))
            .Key("unique_stop_count").Value(static_cast<int>(route->unique_stop_count))
        .EndDict()
        .Build();
    else
        return ConstructNotFoundRequest(id);
}

json::Node ConstructStopRequest(const int id,
                                const std::optional<domain::StopStat>& stop_stat) {
    if (stop_stat) {
        json::Array buses;
        buses.reserve(stop_stat->unique_buses.size());
        for (const domain::BusPtr& bus_ptr : stop_stat->unique_buses)
            buses.push_back(bus_ptr->name);

        return json::Builder{}.StartDict()
            .Key("buses").Value(buses)
            .Key("request_id").Value(id)
        .EndDict()
        .Build();
    } else {
        return ConstructNotFoundRequest(id);
    }
}

void Search(const RequestHandler& handler, const JsonReader& reader) {
    std::vector<json::Node> nodes;
    nodes.reserve(reader.GetStats().size());
    for (const auto& request : reader.GetStats()) {
        const json::Node& type_value = request->at("type");
        const int& id = request->at("id").AsInt();

        if (type_value == "Map") {
            std::ostringstream out;
            handler.RenderMap().Render(out);
            nodes.push_back(
                json::Builder{}.StartDict()
                    .Key("map").Value(out.str())
                    .Key("request_id").Value(id)
                .EndDict()
                .Build()
            );
        } else if (type_value == "Bus") {
            nodes.push_back(ConstructRouteRequest(
                id,
                handler.GetBusStat(request->at("name").AsString())
            ));
        } else if (type_value == "Stop") {
            nodes.push_back(ConstructStopRequest(
                id,
                handler.GetStopStat(request->at("name").AsString())
            ));
        } else {
            ThrowInvalidRequest(std::to_string(id), type_value.AsString());
        }
    }

    json::Print(
        json::Document(json::Builder{}.Value(nodes).Build()),
        std::cout
    );
    std::cout << std::endl;
}

} // end namespace io
} // end namespace transport