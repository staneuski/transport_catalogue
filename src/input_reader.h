#pragma once
#include <iostream>
#include <tuple>
#include <string>
#include <vector>

#include "transport_catalogue.h"

using BusRoute = std::tuple<const std::string, bool, std::vector<std::string>>;

struct Request {
    std::string description;
    std::string content;
};

std::string ReadLine() {
    std::string s;
    std::getline(std::cin, s);
    return s;
}

int ReadLineWithNumber() {
    int result;
    std::cin >> result;
    ReadLine();
    return result;
}

Request ReadRequest(const std::string& delimiter = ": ") {
    std::string s = ReadLine();
    size_t pos = s.find(delimiter);
    return {
        s.substr(0, pos),
        (pos != std::string::npos) ? s.substr(pos + delimiter.size()) : ""
    };
}

std::vector<Request> ReadRequests() {
    const int count = ReadLineWithNumber();
    std::vector<Request> requests;
    requests.reserve(count);
    for (int i = 0; i < count; ++i)
        requests.push_back(ReadRequest());
    return requests;
}

std::vector<std::string> Split(std::string& s,
                               const std::string& delimiter = " ") {
    std::vector<std::string> tokens;

    size_t pos = s.find(delimiter);
    if (pos == std::string::npos)
        return tokens;

    std::string token;
    while (pos != std::string::npos) {
        tokens.push_back(s.substr(0, pos));
        s.erase(0, pos + delimiter.length());
        pos = s.find(delimiter);
    }
    tokens.push_back(s.substr(0, pos));
    s.erase();

    return tokens;
}

Stop ParseStop(Request& request) {
    std::vector<std::string> coords{Split(request.content, ", ")};
    return {
        request.description.substr(request.description.find(" ") + 1),
        {std::stod(coords[0]), std::stod(coords[1])}
    };
}

BusRoute ParseBus(Request& request) {
    std::vector<std::string> route{Split(request.content, " > ")};
    bool is_circular = true;

    if (route.empty()) {
        route = Split(request.content, " - ");
        is_circular = false;
    }

    return {
        request.description.substr(request.description.find(" ") + 1),
        is_circular,
        route
    };
}

void Fill(TransportCatalogue& transport_catalogue) {
    std::vector<Request> requests{ReadRequests()};

    for (Request& request : requests)
        if (request.description.substr(0, 4) == "Stop")
            transport_catalogue.AddStop(ParseStop(request));

    for (Request& request : requests)
        if (request.description.substr(0, 3) == "Bus") {
            const auto& [number, is_circular, route] = ParseBus(request);
            transport_catalogue.AddBus(number, is_circular, route);
        }
}