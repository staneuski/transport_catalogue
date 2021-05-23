#pragma once
#include <iostream>
#include <string>
#include <vector>

#include "transport_catalogue.h"

namespace string_processing {

std::string ReadLine();

int ReadLineWithNumber();

std::vector<std::string> Split(std::string_view sv,
                               const std::string& delimiter = " ");

} // end namespace string_processing

namespace transport {
namespace io {

io::Request ReadRequest(const std::string delimiter = ": ");

std::vector<io::Request> ReadRequests();

inline bool IsStop(const io::Request& request) {
    return request.delimiter == ", ";
}

inline bool IsBus(const io::Request& request) {
    return request.delimiter == " > " || request.delimiter == " - ";
}

void Fill(TransportCatalogue& transport_catalogue);

} // end namespace io
} // end namespace transport