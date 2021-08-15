#include "serialization.h"

#include "domain.h"

namespace transport {
namespace io {

namespace {

std::vector<int> ParseIntegerContainer(std::string_view text) {
    std::vector<int> words;

    const size_t pos_end = text.npos;
    while (true) {
        size_t sep = text.find(',');
        std::string_view word = text.substr(0, sep);

        words.push_back(std::stoi(std::string(word)));

        if (sep == pos_end)
            break;
        else
            text.remove_prefix(sep + 1);
    }
    return words;
}

} // namespace

// ---------- Bufferiser --------------

void Bufferiser::Serialize(std::ostream& out) const {
    pb::Catalogue converted_catalogue;

    const transport::Catalogue& catalogue = request_handler_.GetCatalogue();
    for (const auto& [_, stop_ptr] : catalogue.GetStopsHolder())
        *converted_catalogue.add_stop() = Convert(*stop_ptr);
    for (const auto& [stops, distance] : catalogue.GetDistances())
        *converted_catalogue.add_adjacent_stops() = Convert(stops, distance);
    for (const auto& [_, bus_ptr] : catalogue.GetBusesHolder())
        *converted_catalogue.add_bus() = Convert(*bus_ptr);

    pb::DataBase db;
    *db.mutable_catalogue() = converted_catalogue;
    *db.mutable_map_settings() = Convert(request_handler_.GetRendererSettings());
    db.SerializeToOstream(&out);
}

void Bufferiser::Deserialize(std::istream& in) {
    pb::DataBase db;
    db.ParseFromIstream(&in);

    transport::Catalogue& catalogue = request_handler_.GetCatalogue();
    for (int i = 0; i < db.catalogue().stop_size(); ++i) {
        const pb::domain::Stop& stop = db.catalogue().stop(i);
        catalogue.AddStop({
            stop.name(),
            geo::Coordinates{stop.coords().lat(), stop.coords().lng()},
            static_cast<uint16_t>(stop.wait_time())
        });
    }
    for (int i = 0; i < db.catalogue().adjacent_stops_size(); ++i) {
        const auto& adjacent_stops = db.catalogue().adjacent_stops(i);
        catalogue.MakeAdjacent(
            catalogue.SearchStop(adjacent_stops.stop_name()),
            catalogue.SearchStop(adjacent_stops.adjacent_stop_name()),
            adjacent_stops.distance()
        );
    }
    for (int i = 0; i < db.catalogue().bus_size(); ++i)
        catalogue.AddBus(Convert(db.catalogue().bus(i)));

    request_handler_.SetRendererSettings(Convert(db.map_settings()));
}

pb::domain::Stop Bufferiser::Convert(const domain::Stop& stop) {
    pb::domain::Stop converted;

    converted.set_name(stop.name);
    converted.set_wait_time(stop.wait_time);

    geo::pb::Coordinates coordinates;
    coordinates.set_lat(stop.coords.lat);
    coordinates.set_lng(stop.coords.lng);
    *converted.mutable_coords() = coordinates;

    return converted;
}

pb::domain::AdjacentStops Bufferiser::Convert(
    const Catalogue::AdjacentStops adjacent_stops,
    const int distance
) {
    pb::domain::AdjacentStops converted;

    converted.set_stop_name(adjacent_stops.first->name);
    converted.set_adjacent_stop_name(adjacent_stops.first->name);
    converted.set_distance(distance);

    return converted;
}

pb::renderer::Settings Bufferiser::Convert(
    const renderer::Settings& settings
) {
    pb::renderer::Settings converted;

    converted.set_width(settings.map_sizes.first);
    converted.set_height(settings.map_sizes.second);

    converted.set_padding(settings.padding);
    converted.set_line_width(settings.line_width);
    converted.set_stop_radius(settings.stop_radius);

    converted.set_bus_label_font_size(settings.bus_label.font_size);
    *converted.mutable_bus_label_offset() = Convert(settings.bus_label.offset);

    converted.set_stop_label_font_size(settings.stop_label.font_size);
    *converted.mutable_stop_label_offset() = Convert(settings.stop_label.offset);

    converted.set_underlayer_width(settings.underlayer.width);
    *converted.mutable_underlayer_color() = Convert(settings.underlayer.color);

    for (const svg::Color& color : settings.palette)
        *converted.add_color_palette() = Convert(color);

    return converted;
}

renderer::Settings Bufferiser::Convert(
    const pb::renderer::Settings& settings
) {
    renderer::Settings converted;

    converted.map_sizes = {settings.width(), settings.height()};

    converted.padding = settings.padding();
    converted.line_width = settings.line_width();
    converted.stop_radius = settings.stop_radius();

    converted.bus_label = {
        static_cast<int>(settings.bus_label_font_size()),
        {settings.bus_label_offset().x(), settings.bus_label_offset().y()}
    };
    converted.stop_label = {
        static_cast<int>(settings.stop_label_font_size()),
        {settings.stop_label_offset().x(), settings.stop_label_offset().y()}
    };

    const auto& convert_color = [](const std::string& name) {
        if (name == "none") {
            return svg::Color();
        } else if (name.substr(0, 3) == "rgb") {
            size_t begin = name.find_first_of('(') + 1;
            size_t end = begin + name.find_first_of('(') - 2;
            const auto color = ParseIntegerContainer(name.substr(begin, end));
            return (color.size() == 3)
                   ? svg::Color(svg::Rgb(color[0], color[1], color[2]))
                   : svg::Color(svg::Rgba(color[0], color[1], color[2], color[3]));
        } else {
            return svg::Color(name);
        }
    };

    converted.underlayer = {
        settings.underlayer_width(),
        convert_color(settings.underlayer_color().name())
    };

    converted.palette.reserve(settings.color_palette_size());
    for (int i = 0; i < settings.color_palette_size(); ++i)
        converted.palette.push_back(
            convert_color(settings.color_palette(i).name())
        );

    return converted;
}

pb::domain::Bus Bufferiser::Convert(const domain::Bus& bus) const {
    pb::domain::Bus converted;

    converted.set_name(bus.name);
    converted.set_is_roundtrip(bus.is_roundtrip);
    converted.set_velocity(bus.velocity);

    const transport::Catalogue& catalogue = request_handler_.GetCatalogue();
    for (const domain::StopPtr& stop_ptr : bus.stops)
        converted.add_stop_name(catalogue.SearchStop(stop_ptr->name)->name);

    return converted;
}

domain::Bus Bufferiser::Convert(const pb::domain::Bus& bus) const {
    const transport::Catalogue& catalogue = request_handler_.GetCatalogue();

    std::vector<domain::StopPtr> stops;
    stops.reserve(bus.stop_name_size());
    for (int i = 0; i < bus.stop_name_size(); ++i)
        stops.push_back(catalogue.SearchStop(bus.stop_name(i)));

    return {
        bus.name(),
        stops,
        bus.is_roundtrip(),
        static_cast<uint16_t>(bus.velocity())
    };
}

} // namespace io
} // namespace transport