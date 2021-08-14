#include "serialization.h"

#include "domain.h"

namespace transport {
namespace io {

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