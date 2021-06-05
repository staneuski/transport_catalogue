#pragma once

#include <iostream>
#include <stdexcept>

#include "json/json.h"
#include "request_handler.h"

namespace transport {
namespace io {

inline std::vector<std::string> ConvertNodes(const json::Array& nodes) {
    std::vector<std::string> elements;
    elements.reserve(nodes.size());
    for (const auto& node : nodes)
        elements.push_back(node.AsString());
    return elements;
}

inline std::unordered_map<std::string, int> ConvertNodes(
    const json::Dict& nodes
) {
    std::unordered_map<std::string, int> elements;
    for (const auto& [key, value] : nodes)
        elements[key] = value.AsInt();
    return elements;
}

Requests LoadRequests(std::istream& input);

} // end namespace io
} // end namespace transport