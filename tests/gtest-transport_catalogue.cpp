#include <vector>

#include <gtest/gtest.h>

#include "input_reader.h"
#include "transport_catalogue.h"

using transport::domain::Bus, transport::domain::Stop;
using transport::TransportCatalogue;

TransportCatalogue InitialiseCatalogue() {
    const std::vector<transport::io::Request> stop_requests{
        {"A", {"55.611087", "37.20829", "3900m to B"}, ", "},
        {"B", {"55.595884", "37.209755", "9900m to C", "100m to B"}, ", "},
        {"C", {"55.632761", "37.333324", "9500m to B"}, ", "},
        {"D", {"55.574371", "37.6517", "7500m to I", "1800m to E", "2400m to F"}, ", "},
        {"E", {"55.581065", "37.64839", "750m to F"}, ", "},
        {"F", {"55.587655", "37.645687", "5600m to I", "900m to G"}, ", "},
        {"G", {"55.592028", "37.653656", "1300m to H"}, ", "},
        {"H", {"55.580999", "37.659164", "1200m to D"}, ", "},
        {"I", {"55.595579", "37.605757"}, ", "},
        {"J", {"55.611678", "37.603831"}, ", "}
    };

    const std::vector<transport::io::Request> bus_requests{
        {"828", {"D", "F", "I", "D"}, " > "},
        {"256", {"D", "E", "F", "G", "H", "D"}, " > "},
        {"750", {"A", "B", "B", "C"}, " - "}
    };

    TransportCatalogue transport_catalogue;
    for (const transport::io::Request& request : stop_requests)
        transport_catalogue.AddStop(request);
    for (const transport::io::Request& request : stop_requests)
        transport_catalogue.AbutStops(request);
    for (const transport::io::Request& request : bus_requests)
        transport_catalogue.AddBus(request);

    return transport_catalogue;
}

TEST(TransportCatalogue, SearchStopNotExist) {
    TransportCatalogue transport_catalogue;
    ASSERT_EQ(transport_catalogue.SearchStop("A"), nullptr);
}

TEST(TransportCatalogue, AddStop) {
    TransportCatalogue transport_catalogue;
    transport_catalogue.AddStop({"A", {55.611087, 37.208290}});
    const Stop* ptr{transport_catalogue.SearchStop("A")};

    ASSERT_NE(ptr, nullptr);
    ASSERT_EQ(ptr->name, "A");
    ASSERT_EQ(ptr->coords.lat, 55.611087);
    ASSERT_EQ(ptr->coords.lng, 37.208290);
}

TEST(TransportCatalogue, GetRouteNotExist) {
    const TransportCatalogue transport_catalogue{InitialiseCatalogue()};
    const transport::domain::Route route{transport_catalogue.GetRoute("751")};

    ASSERT_EQ(route.ptr, nullptr);
}

TEST(TransportCatalogue, GetRouteCircular) {
    const TransportCatalogue transport_catalogue{InitialiseCatalogue()};
    const transport::domain::Route route{transport_catalogue.GetRoute("256")};

    ASSERT_NE(route.ptr, nullptr);
    ASSERT_EQ(route.ptr->name, "256");
    ASSERT_EQ(route.ptr->name, route.name);
    ASSERT_EQ(route.stops_count, 6);
    ASSERT_EQ(route.unique_stops_count, 5);
    ASSERT_EQ(route.length, 5950);
    ASSERT_NEAR(route.curvature, 1.36124, 1e-5);

}

TEST(TransportCatalogue, GetRouteNotCircular) {
    const TransportCatalogue transport_catalogue{InitialiseCatalogue()};
    const transport::domain::Route route{transport_catalogue.GetRoute("750")};

    ASSERT_NE(route.ptr, nullptr);
    ASSERT_EQ(route.ptr->name, "750");
    ASSERT_EQ(route.ptr->name, route.name);
    ASSERT_EQ(route.stops_count, 7);
    ASSERT_EQ(route.unique_stops_count, 3);
    ASSERT_EQ(route.length, 27400);
    ASSERT_NEAR(route.curvature, 1.30853, 1e-5);
}

TEST(TransportCatalogue, GetStopNotExist) {
    const TransportCatalogue transport_catalogue{InitialiseCatalogue()};
    transport::domain::StopStat stop_stat = transport_catalogue.GetStop("Z");

    ASSERT_EQ(stop_stat.name, "Z");
    ASSERT_EQ(stop_stat.ptr, nullptr);
    ASSERT_TRUE(stop_stat.unique_buses.empty());
}

TEST(TransportCatalogue, GetStopWithoutBuses) {
    const TransportCatalogue transport_catalogue{InitialiseCatalogue()};
    transport::domain::StopStat stop_stat = transport_catalogue.GetStop("J");

    ASSERT_EQ(stop_stat.name, "J");
    ASSERT_NE(stop_stat.ptr, nullptr);
    ASSERT_TRUE(stop_stat.unique_buses.empty());
}

TEST(TransportCatalogue, GetStop) {
    const TransportCatalogue transport_catalogue{InitialiseCatalogue()};
    transport::domain::StopStat stop_stat = transport_catalogue.GetStop("D");

    std::vector<std::string> bus_names;
    bus_names.reserve(stop_stat.unique_buses.size());
    for (const Bus* bus : stop_stat.unique_buses)
        bus_names.push_back(bus->name);

    ASSERT_EQ(stop_stat.name, "D");
    ASSERT_NE(stop_stat.ptr, nullptr);
    ASSERT_EQ(bus_names, (std::vector<std::string>{"256", "828"}));
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}