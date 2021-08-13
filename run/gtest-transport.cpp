#include <iostream>
#include <fstream>
#include <vector>
#include <stdexcept>

#include <gtest/gtest.h>

#include "catalogue.h"
#include "json_reader.h"

namespace {

static const double NEAR = 1e-5;

using namespace transport;

transport::Catalogue InitialiseDatabase(const std::string_view input_json) {
    transport::Catalogue db;

    if (std::ifstream file(input_json); file) {
        std::stringstream buffer;
        buffer << file.rdbuf();
        io::Populate(db, io::JsonReader{buffer});
    } else {
        throw std::invalid_argument("unable to get " + std::string(input_json));
    }

    return db;
}

namespace gtest_catalogue {

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
    const transport::Catalogue db{InitialiseDatabase("../../resources/(Stop|Bus|Map).json")};
    const std::optional<domain::BusLine> route = db.GetBusLine("751");

    ASSERT_EQ(route, std::nullopt);
}

TEST(TransportCatalogue, GetBusLineCircular) {
    const transport::Catalogue db{InitialiseDatabase("../../resources/(Stop|Bus|Map).json")};
    const std::optional<domain::BusLine> route = db.GetBusLine("256");

    ASSERT_NE(route, std::nullopt);
    ASSERT_EQ(route->stops_count, 6);
    ASSERT_EQ(route->unique_stop_count, 5);
    ASSERT_EQ(route->length, 5950);
    ASSERT_NEAR(route->curvature, 1.36124, NEAR);
}

TEST(TransportCatalogue, GetBusLineNotCircular) {
    const transport::Catalogue db{InitialiseDatabase("../../resources/(Stop|Bus|Map).json")};
    const std::optional<domain::BusLine> route = db.GetBusLine("750");

    ASSERT_NE(route, std::nullopt);
    ASSERT_EQ(route->stops_count, 7);
    ASSERT_EQ(route->unique_stop_count, 3);
    ASSERT_EQ(route->length, 27400);
    ASSERT_NEAR(route->curvature, 1.30853, NEAR);
}

TEST(TransportCatalogue, GetStopNotExist) {
    const transport::Catalogue db{InitialiseDatabase("../../resources/(Stop|Bus|Map).json")};
    const std::optional<domain::StopStat> stop_stat = db.GetStop("Z");

    ASSERT_EQ(stop_stat, std::nullopt);
}

TEST(TransportCatalogue, GetStopWithoutBuses) {
    const transport::Catalogue db{InitialiseDatabase("../../resources/(Stop|Bus|Map).json")};
    const std::optional<domain::StopStat> stop_stat = db.GetStop("J");

    ASSERT_NE(stop_stat, std::nullopt);
    ASSERT_EQ(stop_stat->ptr->name, "J");
    ASSERT_TRUE(stop_stat->unique_buses.empty());
}

TEST(TransportCatalogue, GetStop) {
    const transport::Catalogue db{InitialiseDatabase("../../resources/(Stop|Bus|Map).json")};
    const std::optional<domain::StopStat> stop_stat = db.GetStop("D");

    std::vector<std::string> bus_names;
    bus_names.reserve(stop_stat->unique_buses.size());
    for (const auto& bus : stop_stat->unique_buses)
        bus_names.push_back(bus->name);

    ASSERT_NE(stop_stat, std::nullopt);
    ASSERT_EQ(stop_stat->ptr->name, "D");
    ASSERT_EQ(bus_names, (std::vector<std::string>{"256", "828"}));
}

} // namespace gtest_catalogue

namespace gtest_router {

void AssertRoute(const std::optional<domain::Route> route,
                 const std::pair<std::string_view, std::string_view> direction,
                 const size_t item_count,
                 const double total_time) {
    ASSERT_NE(route, std::nullopt);
    ASSERT_NEAR(route->timedelta, total_time, NEAR);
    ASSERT_EQ(route->edges.size(), item_count);
    ASSERT_EQ(route->edges.front().from->name, direction.first);
    ASSERT_EQ(route->edges.back().to->name, direction.second);
}

void AssertRouteStopEdge(const domain::Edge& edge,
                         const std::string_view stop_name,
                         const int wait_time) {
    ASSERT_EQ(edge.bus, nullptr);
    ASSERT_EQ(edge.from->name, stop_name);
    ASSERT_EQ(edge.timedelta, wait_time);
}

void AssertRouteBusEdge(const domain::Edge& edge,
                        const std::string_view bus_name,
                        const size_t span_count,
                        const double ride_time) {
    ASSERT_NE(edge.bus, nullptr);
    ASSERT_EQ(edge.bus->name, bus_name);
    ASSERT_EQ(edge.stop_count, span_count);
    ASSERT_NEAR(edge.timedelta, ride_time, NEAR);
}

TEST(TransportRouter, GetRouteNotExist) {
    // request_id = 9
    const std::pair<std::string_view, std::string_view> direction = {
        "Biryulyovo Zapadnoye",
        "Tolstopaltsevo"
    };

    const transport::Catalogue db{InitialiseDatabase("../../resources/Route-ex2.json")};
    const std::optional<domain::Route> route = Router(db).GetRoute(
        db.SearchStop(direction.first),
        db.SearchStop(direction.second)
    );

    ASSERT_EQ(route, std::nullopt);
}

TEST(TransportRouter, GetRouteSameStop) {
    // request_id = 4
    const std::pair<std::string_view, std::string_view> direction = {
        "Lipetskaya ulitsa 40",
        "Lipetskaya ulitsa 40"
    };

    const transport::Catalogue db{InitialiseDatabase("../../resources/Route-ex3.json")};
    const std::optional<domain::Route> route = Router(db).GetRoute(
        db.SearchStop(direction.first),
        db.SearchStop(direction.second)
    );

    ASSERT_NE(route, std::nullopt);
    ASSERT_NEAR(route->timedelta, .0, NEAR);
    ASSERT_TRUE(route->edges.empty());
}

TEST(TransportRouter, GetDirectRoute) {
    {
        // request_id = 6
        const std::pair<std::string_view, std::string_view> direction = {
            "Biryulyovo Zapadnoye",
            "Pokrovskaya"
        };

        const transport::Catalogue db{InitialiseDatabase("../../resources/Route-ex2.json")};
        const std::optional<domain::Route> route = Router(db).GetRoute(
            db.SearchStop(direction.first),
            db.SearchStop(direction.second)
        );

        AssertRoute(route, direction, 2u, 11.44);

        AssertRouteStopEdge(route->edges.front(), direction.first, 2);
        AssertRouteBusEdge(route->edges.back(), "828", 4u, 9.44);
    }

    {
        // request_id = 9
        const std::pair<std::string_view, std::string_view> direction = {
            "Biryulyovo Tovarnaya",
            "Prazhskaya"
        };

        const transport::Catalogue db{InitialiseDatabase("../../resources/Route-ex2.json")};
        const std::optional<domain::Route> route = Router(db).GetRoute(
            db.SearchStop(direction.first),
            db.SearchStop(direction.second)
        );

        AssertRoute(route, direction, 2u, 16.32);

        AssertRouteStopEdge(route->edges.front(), direction.first, 2);
        AssertRouteBusEdge(route->edges.back(), "635", 5u, 14.32);
    }

    {
        // request_id = 3
        const std::pair<std::string_view, std::string_view> direction = {
            "Moskvorechye",
            "Zagorye"
        };

        const transport::Catalogue db{InitialiseDatabase("../../resources/Route-ex3.json")};
        const std::optional<domain::Route> route = Router(db).GetRoute(
            db.SearchStop(direction.first),
            db.SearchStop(direction.second)
        );

        AssertRoute(route, direction, 2u, 22.);

        AssertRouteStopEdge(route->edges.front(), direction.first, 2);
        AssertRouteBusEdge(route->edges.back(), "289", 1u, 20.);
    }
}

TEST(TransportRouter, GetRouteWithTransfer) {
    // request_id = 5
    const std::pair<std::string_view, std::string_view> direction = {
        "Biryulyovo Zapadnoye",
        "Apteka"
    };

    const transport::Catalogue db{InitialiseDatabase("../../resources/Route-ex2.json")};
    const std::optional<domain::Route> route = Router(db).GetRoute(
        db.SearchStop(direction.first),
        db.SearchStop(direction.second)
    );

    AssertRoute(route, direction, 4u, 7.42);

    size_t i = 0;
    AssertRouteStopEdge(route->edges.at(i++), "Biryulyovo Zapadnoye", 2);
    AssertRouteBusEdge(route->edges.at(i++), "828", 2u, 3);
    AssertRouteStopEdge(route->edges.at(i++), "Biryusinka", 2);
    AssertRouteBusEdge(route->edges.at(i++), "297", 1u, 0.42);
}

TEST(TransportRouter, GetDirectRouteWithStop) {
    // request_id = 10
    const std::pair<std::string_view, std::string_view> direction = {
        "Apteka",
        "Biryulyovo Tovarnaya"
    };

    const transport::Catalogue db{InitialiseDatabase("../../resources/Route-ex2.json")};
    const std::optional<domain::Route> route = Router(db).GetRoute(
        db.SearchStop(direction.first),
        db.SearchStop(direction.second)
    );

    AssertRoute(route, direction, 4u, 12.04);

    size_t i = 0;
    AssertRouteStopEdge(route->edges.at(i++), direction.first, 2);
    AssertRouteBusEdge(route->edges.at(i++), "297", 1u, 2.84);
    AssertRouteStopEdge(route->edges.at(i++), "Biryulyovo Zapadnoye", 2);
    AssertRouteBusEdge(route->edges.at(i++), "297", 1u, 5.2);
}

TEST(TransportRouter, GetDirectRouteWithReverseSwitch) {
    // request_id = 2
    const std::pair<std::string_view, std::string_view> direction = {
        "Zagorye",
        "Moskvorechye"
    };

    const transport::Catalogue db{InitialiseDatabase("../../resources/Route-ex3.json")};
    const std::optional<domain::Route> route = Router(db).GetRoute(
        db.SearchStop(direction.first),
        db.SearchStop(direction.second)
    );

    AssertRoute(route, direction, 4u, 29.26);

    size_t i = 0;
    AssertRouteStopEdge(route->edges.at(i++), direction.first, 2);
    AssertRouteBusEdge(route->edges.at(i++), "289", 1u, 0.46);
    AssertRouteStopEdge(route->edges.at(i++), "Lipetskaya ulitsa 46", 2);
    AssertRouteBusEdge(route->edges.at(i++), "289", 1u, 24.8);
}

} // namespace gtest_router

namespace gtest_transport {

json::Document LoadJSON(const std::string& json_path) {
    if (std::ifstream file(json_path); file) {
        std::stringstream strm;
        strm << file.rdbuf();
        return json::Load(strm);
    } else {
        throw std::invalid_argument("unable to get " + json_path);
    }
}

json::Document ProcessTransport(const std::string& json_path) {
    std::ifstream file(json_path);
    if (!file)
        throw std::invalid_argument("unable to get " + json_path);

    transport::Catalogue db;
    std::stringstream buffer;
    buffer << file.rdbuf();
    io::JsonReader reader(buffer);
    io::Populate(db, reader);

    return io::Search({db, reader.GenerateMapSettings()}, reader);
}

void CompareOutputs(const std::string& json_path) {
    const json::Array expected_nodes = LoadJSON(json_path + ".out.json").GetRoot().AsArray();
    const json::Array nodes = ProcessTransport(json_path + ".json").GetRoot().AsArray();


    ASSERT_EQ(expected_nodes.size(), nodes.size());
    for (size_t i = 0; i < nodes.size(); ++i) {
        ASSERT_TRUE(expected_nodes.at(i).IsDict());
        ASSERT_TRUE(nodes.at(i).IsDict());

        const json::Dict& expected_dict = expected_nodes.at(i).AsDict();
        const json::Dict& dict = nodes.at(i).AsDict();

        const std::string info = {
            "id = " + std::to_string(dict.at("request_id").AsInt())
            + " (with index=" + std::to_string(i) + ')'
        };

        const auto& ExpectEqInt = [&](const std::string_view type,
                                      const std::string& key) {
            EXPECT_EQ(
                expected_dict.at(key).AsInt(),
                dict.at(key).AsInt()
            ) << "request type = " << type 
              << ", key = " << key << ", " << info;
        };
        const auto& ExpectEqDouble = [&](const std::string_view type,
                                         const std::string& key,
                                         const double acc = NEAR) {
                                             (void) acc;
            EXPECT_NEAR(
                expected_dict.at(key).AsDouble(),
                dict.at(key).AsDouble(),
                dict.at(key).AsDouble() < 1e+6 ? acc : 50
            ) << "request type = " << type
              << ", key = " << key << ", " << info;
        };

        if (dict.find("curvature") != dict.end()) { /* "type": "Bus" */
            ExpectEqDouble("Bus", "curvature", 1e-2);
            ExpectEqDouble("Bus", "route_length", 1e-2);
            ExpectEqInt("Bus", "stop_count");
            ExpectEqInt("Bus", "unique_stop_count");
        } else if (dict.find("buses") != dict.end()) { /* "type": "Stop" */
            const auto& expected_arr = expected_dict.at("buses").AsArray();
            const auto& arr = dict.at("buses").AsArray();

            EXPECT_EQ(
                expected_dict.at("buses").AsArray().size(),
                dict.at("buses").AsArray().size()
            ) << "request type = Stop" << ", " << info;
            for (size_t k = 0; k < arr.size(); ++k)
                EXPECT_EQ(
                    expected_arr.at(k).AsString(),
                    arr.at(k).AsString()
                ) << "request type = Stop" << ", " << info;
        } else if (dict.find("total_time") != dict.end()) { /* "type": "Route" */
            ExpectEqDouble("Route", "total_time", 1e-2);
        }
    }
}

TEST(Transport, CompareJson2) {
    CompareOutputs("../../resources/Route-ex2");
}

TEST(Transport, CompareJson3) {
    CompareOutputs("../../resources/Route-ex3");
}

TEST(Transport, CompareJson4) {
    CompareOutputs("../../resources/Route-ex4");
}

} // namespace gtest_transport

} // namespace

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}