#include "json/json_builder.h"

#include <cassert>
#include <chrono>
#include <iostream>
#include <sstream>

namespace {

static const int ARRAY_SIZE = 1'000;

using namespace json;

int Benchmark() {
    const auto start = std::chrono::steady_clock::now();

    Array arr;
    arr.reserve(ARRAY_SIZE);
    for (int i = 0; i < ARRAY_SIZE; ++i)
        arr.emplace_back(Dict{
            {"int", 42},
            {"double", 42.1},
            {"null", nullptr},
            {"string", "hello"},
            {"array", Array{1, 2, 3}},
            {"bool", false},
            {"map", Dict{{"key", "value"}}},
        });

    std::stringstream strm;
    json::Print(Document{arr}, strm);

    const auto doc = json::Load(strm);
    assert(doc.GetRoot() == arr);

    const auto duration = std::chrono::steady_clock::now() - start;
    return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
}

} // end namespace

int main() {
    using namespace std;

    json::Print(
        json::Document{
            json::Builder{}
                .StartDict()
                    .Key("benchmark_ms"s).Value(Benchmark())
                    .Key("string"s).Value("text"s)
                    .Key("numeric"s).Value(0.5)
                    .Key("bool"s).StartArray().Value(true).Value(false).EndArray()
                .EndDict()
                .Build()
        },
        cout
    );
    cout << endl;

    return 0;
}