#pragma once
#include <iostream>
#include <tuple>
#include <string>
#include <vector>

#include "transport_catalogue.h"

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

std::vector<std::string> Split(std::string_view sv,
                               const std::string& delimiter = " ") {
    std::vector<std::string> cells;

    while (true) {
        size_t pos = sv.find(delimiter);

        const std::string_view& cell = sv.substr(0, pos);
        cells.push_back({cell.begin(), cell.end()});

        if (pos == sv.npos)
            break;
        else
            sv.remove_prefix(pos + delimiter.size());
    }

    return cells;
}

Request ReadRequest(const std::string delimiter = ": ") {
    std::string s = ReadLine();

    Request request;
    const size_t pos = s.find(delimiter);
    if ("Stop" == s.substr(0, 4)) {
        request.name = s.substr(5, pos - 5);
        request.delimiter = ", ";
    } else if ("Bus" == s.substr(0, 3)) {
        request.name = s.substr(4, pos - 4);
        request.delimiter = (s.find(" > ") != std::string::npos) ? " > " : " - ";
    }

    request.contents = Split(
        s.substr(pos + delimiter.size()),
        request.delimiter
    );

    return request;
}

std::vector<Request> ReadRequests() {
    const int count = ReadLineWithNumber();
    std::vector<Request> requests;
    requests.reserve(count);
    for (int i = 0; i < count; ++i)
        requests.push_back(ReadRequest());
    return requests;
}

inline bool IsStop(const Request& request) {
    return request.delimiter == ", ";
}

inline bool IsBus(const Request& request) {
    return request.delimiter == " > " || request.delimiter == " - ";
}

void Fill(TransportCatalogue& transport_catalogue) {
    std::vector<Request> requests{ReadRequests()};

    for (const Request& request : requests)
        if (IsStop(request))
            transport_catalogue.AddStop(request);

    for (const Request& request : requests)
        if (IsStop(request))
            transport_catalogue.AbutStops(request);

    for (const Request& request : requests)
        if (IsBus(request))
            transport_catalogue.AddBus(request);
}