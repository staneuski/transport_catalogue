#include <iostream>
#include <fstream>
#include <vector>
#include <stdexcept>

#include <gtest/gtest.h>

#include "transport/json_reader.h"
#include "transport/transport_catalogue.h"

using transport::domain::Bus, transport::domain::Stop;
using transport::TransportCatalogue;

TransportCatalogue InitialiseDatabase() {
    transport::TransportCatalogue transport_catalogue;

    if (std::ifstream file("../tests/input.json"); file) {
        std::stringstream buffer;
        buffer << file.rdbuf();

        transport::io::Populate(
            transport_catalogue,
            transport::io::JsonReader{buffer}
        );
    } else {
        throw std::invalid_argument("unable to get input.json file");
    }

    return transport_catalogue;
}

TEST(TransportCatalogue, SearchStopNotExist) {
    TransportCatalogue transport_catalogue;
    ASSERT_EQ(transport_catalogue.SearchStop("A"), nullptr);
}

TEST(TransportCatalogue, AddStop) {
    TransportCatalogue transport_catalogue;
    transport_catalogue.AddStop({"A", {55.611087, 37.208290}});
    transport::domain::StopPtr ptr{transport_catalogue.SearchStop("A")};

    ASSERT_NE(ptr, nullptr);
    ASSERT_EQ(ptr->name, "A");
    ASSERT_EQ(ptr->coords.lat, 55.611087);
    ASSERT_EQ(ptr->coords.lng, 37.208290);
}

TEST(TransportCatalogue, GetRouteNotExist) {
    const TransportCatalogue transport_catalogue{InitialiseDatabase()};
    const transport::domain::Route route{transport_catalogue.GetRoute("751")};

    ASSERT_EQ(route.ptr, nullptr);
}

TEST(TransportCatalogue, GetRouteCircular) {
    const TransportCatalogue transport_catalogue{InitialiseDatabase()};
    const transport::domain::Route route{transport_catalogue.GetRoute("256")};

    ASSERT_NE(route.ptr, nullptr);
    ASSERT_EQ(route.ptr->name, "256");
    ASSERT_EQ(route.ptr->name, route.name);
    ASSERT_EQ(route.stops_count, 6);
    ASSERT_EQ(route.unique_stop_count, 5);
    ASSERT_EQ(route.length, 5950);
    ASSERT_NEAR(route.curvature, 1.36124, 1e-5);

}

TEST(TransportCatalogue, GetRouteNotCircular) {
    const TransportCatalogue transport_catalogue{InitialiseDatabase()};
    const transport::domain::Route route{transport_catalogue.GetRoute("750")};

    ASSERT_NE(route.ptr, nullptr);
    ASSERT_EQ(route.ptr->name, "750");
    ASSERT_EQ(route.ptr->name, route.name);
    ASSERT_EQ(route.stops_count, 7);
    ASSERT_EQ(route.unique_stop_count, 3);
    ASSERT_EQ(route.length, 27400);
    ASSERT_NEAR(route.curvature, 1.30853, 1e-5);
}

TEST(TransportCatalogue, GetStopNotExist) {
    const TransportCatalogue transport_catalogue{InitialiseDatabase()};
    transport::domain::StopStat stop_stat = transport_catalogue.GetStop("Z");

    ASSERT_EQ(stop_stat.name, "Z");
    ASSERT_EQ(stop_stat.ptr, nullptr);
    ASSERT_TRUE(stop_stat.unique_buses.empty());
}

TEST(TransportCatalogue, GetStopWithoutBuses) {
    const TransportCatalogue transport_catalogue{InitialiseDatabase()};
    transport::domain::StopStat stop_stat = transport_catalogue.GetStop("J");

    ASSERT_EQ(stop_stat.name, "J");
    ASSERT_NE(stop_stat.ptr, nullptr);
    ASSERT_TRUE(stop_stat.unique_buses.empty());
}

TEST(TransportCatalogue, GetStop) {
    const TransportCatalogue transport_catalogue{InitialiseDatabase()};
    transport::domain::StopStat stop_stat = transport_catalogue.GetStop("D");

    std::vector<std::string> bus_names;
    bus_names.reserve(stop_stat.unique_buses.size());
    for (const auto& bus : stop_stat.unique_buses)
        bus_names.push_back(bus->name);

    ASSERT_EQ(stop_stat.name, "D");
    ASSERT_NE(stop_stat.ptr, nullptr);
    ASSERT_EQ(bus_names, (std::vector<std::string>{"256", "828"}));
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}