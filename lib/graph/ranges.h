#pragma once

#include <iterator>
#include <string_view>
#include <unordered_map>
#include <unordered_set>

namespace graph {

template <typename It>
class Range {
public:
    using ValueType = typename std::iterator_traits<It>::value_type;

public:
    Range(It begin, It end) : begin_(begin), end_(end) {}

    inline It begin() const {
        return begin_;
    }

    inline It end() const {
        return end_;
    }

private:
    It begin_, end_;
};

template <typename C>
auto AsRange(const C& container) {
    return Range{container.begin(), container.end()};
}

} // namespace graph