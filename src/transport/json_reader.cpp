#include "json_reader.h"

namespace transport {
namespace io {

void JsonReader::ParseBuses() {
    const json::Array& base_requests = requests_.at("base_requests").AsArray();

    buses_.reserve(base_requests.size()/2u);
    for (const auto& base_request : base_requests) {
        const json::Dict& request = base_request.AsDict();

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
        const json::Dict& request = base_request.AsDict();

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
        const json::Dict& request = request_node.AsDict();

        const json::Node& type_name = request.at("type");
        if ("Bus" == type_name || "Stop" == type_name || "Map" == type_name)
            stats_.push_back(std::make_unique<const json::Dict>(request));
        else
            throw std::invalid_argument(
                "unable to load stat request type '" + type_name.AsString() + "'"
            );
    }
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
        const std::string& type = request->at("type").AsString();
        const int& id = request->at("id").AsInt();

        if ("Map" == type) {
            std::ostringstream out;
            handler.RenderMap().Render(out);
            nodes.push_back(
                json::Builder{}.StartDict()
                    .Key("map").Value(out.str())
                    .Key("request_id").Value(id)
                .EndDict()
                .Build()
            );
        } else if ("Bus" == type) {
            nodes.push_back(ConstructRouteRequest(
                id,
                handler.GetBusStat(request->at("name").AsString())
            ));
        } else if ("Stop" == type) {
            nodes.push_back(ConstructStopRequest(
                id,
                handler.GetStopStat(request->at("name").AsString())
            ));
        } else {
            throw std::invalid_argument(
                "unable to load request's type: '" + type
                + "' (id=" + std::to_string(id) + ")"
            );
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