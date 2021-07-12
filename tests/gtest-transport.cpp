#include <iostream>
#include <fstream>
#include <vector>
#include <stdexcept>

#include <gtest/gtest.h>

#include "transport/catalogue.h"
#include "transport/json_reader.h"

namespace {

using namespace transport;

transport::Catalogue InitialiseDatabase(
    const std::string_view input_json = "../tests/input.json"
) {
    transport::Catalogue db;

    if (std::ifstream file(input_json); file) {
        std::stringstream buffer;
        buffer << file.rdbuf();
        io::Populate(db, io::JsonReader{buffer});
    } else {
        throw std::invalid_argument("unable to get input_json file");
    }

    return db;
}

TEST(TransportCatalogue, SearchStopNotExist) {
    transport::Catalogue db;
    ASSERT_EQ(db.SearchStop("A"), nullptr);
}

TEST(TransportCatalogue, AddStop) {
    transport::Catalogue db;
    db.AddStop(domain::Stop{.name = "A", .coords = {55.611087, 37.208290}});

    domain::StopPtr ptr{db.SearchStop("A")};

    ASSERT_NE(ptr, nullptr);
    ASSERT_EQ(ptr->name, "A");
    ASSERT_EQ(ptr->coords.lat, 55.611087);
    ASSERT_EQ(ptr->coords.lng, 37.208290);
}

TEST(TransportCatalogue, GetBusLineNotExist) {
    const transport::Catalogue db{InitialiseDatabase()};
    const std::optional<domain::BusLine> route = db.GetBusLine("751");

    ASSERT_EQ(route, std::nullopt);
}

TEST(TransportCatalogue, GetBusLineCircular) {
    const transport::Catalogue db{InitialiseDatabase()};
    const std::optional<domain::BusLine> route = db.GetBusLine("256");

    ASSERT_NE(route, std::nullopt);
    ASSERT_EQ(route->stops_count, 6);
    ASSERT_EQ(route->unique_stop_count, 5);
    ASSERT_EQ(route->length, 5950);
    ASSERT_NEAR(route->curvature, 1.36124, 1e-5);

}

TEST(TransportCatalogue, GetBusLineNotCircular) {
    const transport::Catalogue db{InitialiseDatabase()};
    const std::optional<domain::BusLine> route = db.GetBusLine("750");

    ASSERT_NE(route, std::nullopt);
    ASSERT_EQ(route->stops_count, 7);
    ASSERT_EQ(route->unique_stop_count, 3);
    ASSERT_EQ(route->length, 27400);
    ASSERT_NEAR(route->curvature, 1.30853, 1e-5);
}

TEST(TransportCatalogue, GetStopNotExist) {
    const transport::Catalogue db{InitialiseDatabase()};
    const std::optional<domain::StopStat> stop_stat = db.GetStop("Z");

    ASSERT_EQ(stop_stat, std::nullopt);
}

TEST(TransportCatalogue, GetStopWithoutBuses) {
    const transport::Catalogue db{InitialiseDatabase()};
    const std::optional<domain::StopStat> stop_stat = db.GetStop("J");

    ASSERT_NE(stop_stat, std::nullopt);
    ASSERT_EQ(stop_stat->ptr->name, "J");
    ASSERT_TRUE(stop_stat->unique_buses.empty());
}

TEST(TransportCatalogue, GetStop) {
    const transport::Catalogue db{InitialiseDatabase()};
    const std::optional<domain::StopStat> stop_stat = db.GetStop("D");

    std::vector<std::string> bus_names;
    bus_names.reserve(stop_stat->unique_buses.size());
    for (const auto& bus : stop_stat->unique_buses)
        bus_names.push_back(bus->name);

    ASSERT_NE(stop_stat, std::nullopt);
    ASSERT_EQ(stop_stat->ptr->name, "D");
    ASSERT_EQ(bus_names, (std::vector<std::string>{"256", "828"}));
}

} // end namespace

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}