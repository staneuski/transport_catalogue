#include "json/json.h"

#include <cassert>
#include <chrono>
#include <sstream>
#include <string_view>

#include <gtest/gtest.h>

using namespace json;
using namespace std::literals;

namespace {

json::Document LoadJSON(const std::string& s) {
    std::istringstream strm(s);
    return json::Load(strm);
}

std::string Print(const Node& node) {
    std::ostringstream out;
    Print(Document{node}, out);
    return out.str();
}

TEST(JSON, Null) {
    Node null_node;
    ASSERT_TRUE(null_node.IsNull());

    Node null_node1{nullptr};
    ASSERT_TRUE(null_node1.IsNull());

    ASSERT_EQ(Print(null_node), "null"s);

    const Node node = LoadJSON("null"s).GetRoot();
    ASSERT_TRUE(node.IsNull());
    ASSERT_EQ(node, null_node);
}

TEST(JSON, Numbers) {
    Node int_node{42};
    ASSERT_TRUE(int_node.IsInt());
    ASSERT_EQ(int_node.AsInt(), 42);
    // целые числа являются подмножеством чисел с плавающей запятой
    ASSERT_TRUE(int_node.IsDouble());
    // Когда узел хранит int, можно получить соответствующее ему double-значение
    ASSERT_EQ(int_node.AsDouble(), 42.0);
    ASSERT_TRUE(!int_node.IsPureDouble());

    Node dbl_node{123.45};
    ASSERT_TRUE(dbl_node.IsDouble());
    ASSERT_EQ(dbl_node.AsDouble(), 123.45);
    ASSERT_TRUE(dbl_node.IsPureDouble()); // Значение содержит число с плавающей запятой
    ASSERT_TRUE(!dbl_node.IsInt());

    ASSERT_EQ(Print(int_node), "42"s);
    ASSERT_EQ(Print(dbl_node), "123.45"s);

    ASSERT_EQ(LoadJSON("42"s).GetRoot(), int_node);
    ASSERT_EQ(LoadJSON("123.45"s).GetRoot(), dbl_node);
    ASSERT_EQ(LoadJSON("0.25"s).GetRoot().AsDouble(), 0.25);
    ASSERT_EQ(LoadJSON("3e5"s).GetRoot().AsDouble(), 3e5);
    ASSERT_EQ(LoadJSON("1.2e-5"s).GetRoot().AsDouble(), 1.2e-5);
    ASSERT_EQ(LoadJSON("1.2e+5"s).GetRoot().AsDouble(), 1.2e5);
    ASSERT_EQ(LoadJSON("-123456"s).GetRoot().AsInt(), -123456);
}

TEST(JSON, Strings) {
    Node str_node{"Hello, \"everybody\""s};
    ASSERT_TRUE(str_node.IsString());
    ASSERT_EQ(str_node.AsString(), "Hello, \"everybody\""s);

    ASSERT_TRUE(!str_node.IsInt());
    ASSERT_TRUE(!str_node.IsDouble());

    ASSERT_EQ(Print(str_node), "\"Hello, \\\"everybody\\\"\""s);

    ASSERT_EQ(LoadJSON(Print(str_node)).GetRoot(), str_node);
}

TEST(JSON, Bool) {
    Node true_node{true};
    ASSERT_TRUE(true_node.IsBool());
    ASSERT_TRUE(true_node.AsBool());

    Node false_node{false};
    ASSERT_TRUE(false_node.IsBool());
    ASSERT_TRUE(!false_node.AsBool());

    ASSERT_EQ(Print(true_node), "true"s);
    ASSERT_EQ(Print(false_node), "false"s);

    ASSERT_EQ(LoadJSON("true"s).GetRoot(), true_node);
    ASSERT_EQ(LoadJSON("false"s).GetRoot(), false_node);
}

TEST(JSON, Array) {
    Node arr_node{Array{1, 1.23, "Hello"s, "true"s}};
    ASSERT_TRUE(arr_node.IsArray());
    const Array& arr = arr_node.AsArray();
    ASSERT_EQ(arr.size(), 4);
    ASSERT_EQ(arr.at(0).AsInt(), 1);

    ASSERT_EQ(LoadJSON("[1, 1.23, \"Hello\", \"true\"]"s).GetRoot(), arr_node);
    ASSERT_EQ(LoadJSON(Print(arr_node)).GetRoot(), arr_node);
}

TEST(JSON, Map) {
    Node dict_node{Dict{{"key1"s, "value1"s}, {"key2"s, 42}}};
    ASSERT_TRUE(dict_node.IsMap());
    const Dict& dict = dict_node.AsMap();
    ASSERT_EQ(dict.size(), 2);
    ASSERT_EQ(dict.at("key1"s).AsString(), "value1"s);
    ASSERT_EQ(dict.at("key2"s).AsInt(), 42);

    ASSERT_EQ(LoadJSON("{\"key1\":\"value1\", \"key2\":42}"s).GetRoot(), dict_node);
    ASSERT_EQ(LoadJSON(Print(dict_node)).GetRoot(), dict_node);
}

} // end namespace

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}