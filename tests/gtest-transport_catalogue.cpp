#include <vector>

#include <gtest/gtest.h>

#include "input_reader.h"
#include "transport_catalogue.h"

TransportCatalogue InitialiseCatalogue() {
    const std::vector<Request> stop_requests{
        {"A", {"55.611087", "37.208290"}, ", "},
        {"B", {"55.595884", "37.209755"}, ", "},
        {"C", {"55.632761", "37.333324"}, ", "},
        {"D", {"55.574371", "37.651700"}, ", "},
        {"E", {"55.581065", "37.648390"}, ", "},
        {"F", {"55.587655", "37.645687"}, ", "},
        {"G", {"55.592028", "37.653656"}, ", "},
        {"H", {"55.580999", "37.659164"}, ", "},
        {"I", {"55.581102", "37.659203"}, ", "}
    };
    const std::vector<Request> bus_requests{
        {"828", {"D", "F", "H", "D"}, " > "},
        {"750", {"A", "B", "C"}, " - "},
        {"256", {"D", "E", "F", "G", "H", "D"}, " > "},
        {"11", {"A", "A", "B", "B"}, " - "},
        {"22", {"B", "A", "B", "A"}, " - "},
        {"33t", {"A", "A", "A", "A"}, " > "},
        {"33f", {"A", "A", "A", "A"}, " - "},
        {"44t", {"A", "B", "B", "A"}, " > "},
        {"66f", {"A", "B", "B", "A"}, " - "}
    };

    TransportCatalogue transport_catalogue;
    for (const Request& request : stop_requests)
        transport_catalogue.AddStop(request);
    for (const Request& request : bus_requests)
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
    const Route route{transport_catalogue.GetRoute("751")};

    ASSERT_EQ(route.ptr, nullptr);
}

TEST(TransportCatalogue, GetRouteCircular) {
    {
        const TransportCatalogue transport_catalogue{InitialiseCatalogue()};
        const Route route{transport_catalogue.GetRoute("256")};

        ASSERT_NE(route.ptr, nullptr);
        ASSERT_EQ(route.ptr->name, "256");
        ASSERT_EQ(route.ptr->name, route.name);
        ASSERT_EQ(route.stops_count, 6);
        ASSERT_EQ(route.unique_stops_count, 5);
        ASSERT_NEAR(route.length, 4371.0172, 1e-4);
    }
    {
        const TransportCatalogue transport_catalogue{InitialiseCatalogue()};
        const Route route{transport_catalogue.GetRoute("33t")};

        ASSERT_NE(route.ptr, nullptr);
        ASSERT_EQ(route.ptr->name, "33t");
        ASSERT_EQ(route.ptr->name, route.name);
        ASSERT_EQ(route.stops_count, 4);
        ASSERT_EQ(route.unique_stops_count, 1);
    }
    {
        const TransportCatalogue transport_catalogue{InitialiseCatalogue()};
        const Route route{transport_catalogue.GetRoute("44t")};

        ASSERT_NE(route.ptr, nullptr);
        ASSERT_EQ(route.ptr->name, "44t");
        ASSERT_EQ(route.ptr->name, route.name);
        ASSERT_EQ(route.stops_count, 4);
        ASSERT_EQ(route.unique_stops_count, 2);
    }
}

TEST(TransportCatalogue, GetRouteNotCircular) {
    {
        const TransportCatalogue transport_catalogue{InitialiseCatalogue()};
        const Route route{transport_catalogue.GetRoute("750")};

        ASSERT_NE(route.ptr, nullptr);
        ASSERT_EQ(route.ptr->name, "750");
        ASSERT_EQ(route.ptr->name, route.name);
        ASSERT_EQ(route.stops_count, 5);
        ASSERT_EQ(route.unique_stops_count, 3);
        ASSERT_NEAR(route.length, 20939.483, 1e-4);
    }
    {
        const TransportCatalogue transport_catalogue{InitialiseCatalogue()};
        const Route route{transport_catalogue.GetRoute("11")};

        ASSERT_NE(route.ptr, nullptr);
        ASSERT_EQ(route.ptr->name, "11");
        ASSERT_EQ(route.ptr->name, route.name);
        ASSERT_EQ(route.stops_count, 7);
        ASSERT_EQ(route.unique_stops_count, 2);
    }
    {
        const TransportCatalogue transport_catalogue{InitialiseCatalogue()};
        const Route route{transport_catalogue.GetRoute("22")};

        ASSERT_NE(route.ptr, nullptr);
        ASSERT_EQ(route.ptr->name, "22");
        ASSERT_EQ(route.ptr->name, route.name);
        ASSERT_EQ(route.stops_count, 7);
        ASSERT_EQ(route.unique_stops_count, 2);
    }
    {
        const TransportCatalogue transport_catalogue{InitialiseCatalogue()};
        const Route route{transport_catalogue.GetRoute("33f")};

        ASSERT_NE(route.ptr, nullptr);
        ASSERT_EQ(route.ptr->name, "33f");
        ASSERT_EQ(route.ptr->name, route.name);
        ASSERT_EQ(route.stops_count, 7);
        ASSERT_EQ(route.unique_stops_count, 1);
    }
    {
        const TransportCatalogue transport_catalogue{InitialiseCatalogue()};
        const Route route{transport_catalogue.GetRoute("66f")};

        ASSERT_NE(route.ptr, nullptr);
        ASSERT_EQ(route.ptr->name, "66f");
        ASSERT_EQ(route.ptr->name, route.name);
        ASSERT_EQ(route.stops_count, 7);
        ASSERT_EQ(route.unique_stops_count, 2);
    }
}

TEST(TransportCatalogue, GetStopNotExist) {
    const TransportCatalogue transport_catalogue{InitialiseCatalogue()};
    StopStat stop_stat = transport_catalogue.GetStop("Z");

    ASSERT_EQ(stop_stat.name, "Z");
    ASSERT_EQ(stop_stat.ptr, nullptr);
    ASSERT_TRUE(stop_stat.unique_buses.empty());
}


TEST(TransportCatalogue, GetStopWithoutBuses) {
    const TransportCatalogue transport_catalogue{InitialiseCatalogue()};
    StopStat stop_stat = transport_catalogue.GetStop("I");

    ASSERT_EQ(stop_stat.name, "I");
    ASSERT_NE(stop_stat.ptr, nullptr);
    ASSERT_TRUE(stop_stat.unique_buses.empty());
}

TEST(TransportCatalogue, GetStop) {
    const TransportCatalogue transport_catalogue{InitialiseCatalogue()};
    StopStat stop_stat = transport_catalogue.GetStop("D");

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