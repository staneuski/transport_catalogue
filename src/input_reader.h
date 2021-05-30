#pragma once
#include <iostream>
#include <string>
#include <vector>

namespace string_processing {

std::string ReadLine();

int ReadLineWithNumber();

std::vector<std::string> Split(std::string_view sv,
                               const std::string& delimiter = " ");

} // end namespace string_processing

namespace transport {

class TransportCatalogue;

namespace io {

struct Request {
    std::string name;
    std::vector<std::string> contents;
    std::string delimiter;
};

Request ReadRequest(const std::string delimiter = ": ");

std::vector<Request> ReadRequests();

inline bool IsStop(const Request& request) {
    return request.delimiter == ", ";
}

inline bool IsBus(const Request& request) {
    return request.delimiter == " > " || request.delimiter == " - ";
}

void Fill(TransportCatalogue& transport_catalogue);

} // end namespace io
} // end namespace transport