#include <vector>

#include <gtest/gtest.h>

#include "input_reader.h"
#include "transport_catalogue.h"

TransportCatalogue InitialiseCatalogue() {
    TransportCatalogue transport_catalogue;

    transport_catalogue.AddStop({"A", {55.611087, 37.208290}});
    transport_catalogue.AddStop({"B", {55.595884, 37.209755}});
    transport_catalogue.AddStop({"C", {55.632761, 37.333324}});
    transport_catalogue.AddStop({"D", {55.574371, 37.651700}});
    transport_catalogue.AddStop({"E", {55.581065, 37.648390}});
    transport_catalogue.AddStop({"F", {55.587655, 37.645687}});
    transport_catalogue.AddStop({"G", {55.592028, 37.653656}});
    transport_catalogue.AddStop({"H", {55.580999, 37.659164}});

    transport_catalogue.AddBus("256", true, {"D", "E", "F", "G", "H", "D"});
    transport_catalogue.AddBus("750", false, {"A", "B", "C"});

    transport_catalogue.AddBus("11", false, {"A", "A", "B", "B"});
    transport_catalogue.AddBus("22", false, {"B", "A", "B", "A"});

    transport_catalogue.AddBus("33t", true, {"A", "A", "A", "A"});
    transport_catalogue.AddBus("33f", false, {"A", "A", "A", "A"});

    transport_catalogue.AddBus("44t", true, {"A", "B", "B", "A"});
    transport_catalogue.AddBus("66f", false, {"A", "B", "B", "A"});

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

    ASSERT_EQ(route.bus_ptr, nullptr);
}

TEST(TransportCatalogue, GetRouteCircular) {
    {
        const TransportCatalogue transport_catalogue{InitialiseCatalogue()};
        const Route route{transport_catalogue.GetRoute("256")};

        ASSERT_NE(route.bus_ptr, nullptr);
        ASSERT_EQ(route.bus_ptr->name, "256");
        ASSERT_EQ(route.bus_ptr->name, route.name);
        ASSERT_EQ(route.stops, 6);
        ASSERT_EQ(route.unique_stops, 5);
        ASSERT_NEAR(route.length, 4371.0172, 1e-4);
    }
    {
        const TransportCatalogue transport_catalogue{InitialiseCatalogue()};
        const Route route{transport_catalogue.GetRoute("33t")};

        ASSERT_NE(route.bus_ptr, nullptr);
        ASSERT_EQ(route.bus_ptr->name, "33t");
        ASSERT_EQ(route.bus_ptr->name, route.name);
        ASSERT_EQ(route.stops, 4);
        ASSERT_EQ(route.unique_stops, 1);
    }
    {
        const TransportCatalogue transport_catalogue{InitialiseCatalogue()};
        const Route route{transport_catalogue.GetRoute("44t")};

        ASSERT_NE(route.bus_ptr, nullptr);
        ASSERT_EQ(route.bus_ptr->name, "44t");
        ASSERT_EQ(route.bus_ptr->name, route.name);
        ASSERT_EQ(route.stops, 4);
        ASSERT_EQ(route.unique_stops, 2);
    }
}

TEST(TransportCatalogue, GetRouteNotCircular) {
    {
        const TransportCatalogue transport_catalogue{InitialiseCatalogue()};
        const Route route{transport_catalogue.GetRoute("750")};

        ASSERT_NE(route.bus_ptr, nullptr);
        ASSERT_EQ(route.bus_ptr->name, "750");
        ASSERT_EQ(route.bus_ptr->name, route.name);
        ASSERT_EQ(route.stops, 5);
        ASSERT_EQ(route.unique_stops, 3);
        ASSERT_NEAR(route.length, 20939.483, 1e-4);
    }
    {
        const TransportCatalogue transport_catalogue{InitialiseCatalogue()};
        const Route route{transport_catalogue.GetRoute("11")};

        ASSERT_NE(route.bus_ptr, nullptr);
        ASSERT_EQ(route.bus_ptr->name, "11");
        ASSERT_EQ(route.bus_ptr->name, route.name);
        ASSERT_EQ(route.stops, 7);
        ASSERT_EQ(route.unique_stops, 2);
    }
    {
        const TransportCatalogue transport_catalogue{InitialiseCatalogue()};
        const Route route{transport_catalogue.GetRoute("22")};

        ASSERT_NE(route.bus_ptr, nullptr);
        ASSERT_EQ(route.bus_ptr->name, "22");
        ASSERT_EQ(route.bus_ptr->name, route.name);
        ASSERT_EQ(route.stops, 7);
        ASSERT_EQ(route.unique_stops, 2);
    }
    {
        const TransportCatalogue transport_catalogue{InitialiseCatalogue()};
        const Route route{transport_catalogue.GetRoute("33f")};

        ASSERT_NE(route.bus_ptr, nullptr);
        ASSERT_EQ(route.bus_ptr->name, "33f");
        ASSERT_EQ(route.bus_ptr->name, route.name);
        ASSERT_EQ(route.stops, 7);
        ASSERT_EQ(route.unique_stops, 1);
    }
    {
        const TransportCatalogue transport_catalogue{InitialiseCatalogue()};
        const Route route{transport_catalogue.GetRoute("66f")};

        ASSERT_NE(route.bus_ptr, nullptr);
        ASSERT_EQ(route.bus_ptr->name, "66f");
        ASSERT_EQ(route.bus_ptr->name, route.name);
        ASSERT_EQ(route.stops, 7);
        ASSERT_EQ(route.unique_stops, 2);
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}