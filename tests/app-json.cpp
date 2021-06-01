#include "json/json.h"

#include <cassert>
#include <chrono>
#include <sstream>
#include <string_view>

namespace {

static const int ARRAY_SIZE = 1'000;

using namespace json;

void Benchmark() {
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
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count()
              << "ms" << std::endl;
}

} // end namespace

int main() {
    Benchmark();

    json::Dict map{{{"null", nullptr}}};
    Print(json::Document{map}, std::cout);
    std::cout << std::endl;

    std::stringstream strm;
    json::Print(json::Document{map}, strm);

    const auto doc = json::Load(strm);
    assert(doc.GetRoot() == map);

    return 0;
}