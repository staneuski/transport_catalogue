#include <vector>

#include <gtest/gtest.h>

#include "transport/input_handler.h"
#include "transport/transport_catalogue.h"

using transport::domain::Bus, transport::domain::Stop;
using transport::TransportCatalogue;

TransportCatalogue InitialiseCatalogue() {
    transport::io::Requests requests {
        {
            {"828", {"D", "F", "I", "D"}, true},
            {"256", {"D", "E", "F", "G", "H", "D"}, true},
            {"750", {"A", "B", "B", "C"}, false}
        },
        {
            {"A", 55.611087, 37.20829, {{"B", 3900}}},
            {"B", 55.595884, 37.209755, {{"C", 9900}, {"B", 100}}},
            {"C", 55.632761, 37.333324, {{"B", 9500}}},
            {"D", 55.574371, 37.6517, {{"I", 7500}, {"E", 1800}, {"F", 2400}}},
            {"E", 55.581065, 37.64839, {{"F", 750}}},
            {"F", 55.587655, 37.645687, {{"I", 5600}, {"G", 900}}},
            {"G", 55.592028, 37.653656, {{"H", 1300}}},
            {"H", 55.580999, 37.659164, {{"D", 1200}}},
            {"I", 55.595579, 37.605757, {}},
            {"J", 55.611678, 37.603831, {}}
        },
        {}
    };

    TransportCatalogue transport_catalogue;
    for (const transport::io::Request::Stop& request : requests.stops)
        transport_catalogue.AddStop(request);
    for (const transport::io::Request::Stop& request : requests.stops)
        transport_catalogue.MakeAdjacent(request);

    for (const transport::io::Request::Bus& request : requests.buses)
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
    transport::domain::StopPtr ptr{transport_catalogue.SearchStop("A")};

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