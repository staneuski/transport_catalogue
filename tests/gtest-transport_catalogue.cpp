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

    transport_catalogue.AddBus(256, true, {"D", "E", "F", "G", "H", "D"});
    transport_catalogue.AddBus(750, false, {"A", "B", "C"});

    transport_catalogue.AddBus(0, false, {"A", "B"});
    transport_catalogue.AddBus(1, false, {"A", "A", "B", "B"});
    transport_catalogue.AddBus(2, false, {"B", "A", "B", "A"});

    transport_catalogue.AddBus(3, true, {"A", "A", "A", "A"});
    transport_catalogue.AddBus(4, false, {"A", "A", "A", "A"});

    transport_catalogue.AddBus(5, true, {"A", "B", "B", "A"});
    transport_catalogue.AddBus(6, false, {"A", "B", "B", "A"});

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

TEST(TransportCatalogue, AddBus) {
    TransportCatalogue transport_catalogue;
    transport_catalogue.AddStop({"A", {55.611087, 37.208290}});
    const Stop* ptr{transport_catalogue.SearchStop("A")};

    ASSERT_NE(ptr, nullptr);
    ASSERT_EQ(ptr->name, "A");
    ASSERT_EQ(ptr->coords.lat, 55.611087);
    ASSERT_EQ(ptr->coords.lng, 37.208290);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}