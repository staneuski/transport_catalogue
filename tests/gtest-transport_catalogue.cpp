#include <vector>

#include <gtest/gtest.h>

#include "input_reader.h"
#include "transport_catalogue.h"

TransportCatalogue InitialiseCatalogue() {
    std::vector<Stop> stops{
        {"Tolstopaltsevo", {55.611087, 37.208290}},
        {"Marushkino", {55.595884, 37.209755}},
        {"Rasskazovka", {55.632761, 37.333324}},
        {"Biryulyovo Zapadnoye", {55.574371, 37.651700}},
        {"Biryusinka", {55.581065, 37.648390}},
        {"Universam", {55.587655, 37.645687}},
        {"Biryulyovo Tovarnaya", {55.592028, 37.653656}},
        {"Biryulyovo Passazhirskaya", {55.580999, 37.659164}}
    };

    std::vector<BusRoute> bus_routes{
        {256, {"Biryulyovo Zapadnoye",
               "Biryusinka",
               "Universam",
               "Biryulyovo Tovarnaya",
               "Biryulyovo Passazhirskaya",
               "Biryulyovo Zapadnoye"}},
        {750, {"Tolstopaltsevo", "Marushkino", "Rasskazovka"}}
    }; 

    TransportCatalogue transport_catalogue;
    for (auto& stop : stops)
        transport_catalogue.AddStop(stop);
    for (auto& [number, route] : bus_routes)
        transport_catalogue.AddBus(number, route);

    return transport_catalogue;
}

TEST(TransportCatalogue, SearchStop) {
    TransportCatalogue transport_catalogue;
    ASSERT_EQ(transport_catalogue.SearchStop("Tolstopaltsevo"), nullptr);
}

TEST(TransportCatalogue, AddStop) {
    TransportCatalogue transport_catalogue;
    transport_catalogue.AddStop({"Tolstopaltsevo", {55.611087, 37.208290}});
    const Stop* ptr{transport_catalogue.SearchStop("Tolstopaltsevo")};

    ASSERT_NE(ptr, nullptr);
    ASSERT_EQ(ptr->name, "Tolstopaltsevo");
    ASSERT_EQ(ptr->coords.lat, 55.611087);
    ASSERT_EQ(ptr->coords.lng, 37.208290);
}

TEST(TransportCatalogue, AddBus) {
    TransportCatalogue transport_catalogue;
    transport_catalogue.AddStop({"Tolstopaltsevo", {55.611087, 37.208290}});
    const Stop* ptr{transport_catalogue.SearchStop("Tolstopaltsevo")};

    ASSERT_NE(ptr, nullptr);
    ASSERT_EQ(ptr->name, "Tolstopaltsevo");
    ASSERT_EQ(ptr->coords.lat, 55.611087);
    ASSERT_EQ(ptr->coords.lng, 37.208290);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}