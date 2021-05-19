#include <iostream>
#include <regex>
#include <string>

#include "transport_catalogue.h"

using namespace std::literals;

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

std::vector<std::string> SplitIntoStopsNames(std::string_view sv) {
    static std::regex const stop_regex("\\s+(>|-)\\s+");
    std::regex_token_iterator<std::string_view::iterator> i(
        sv.begin(),
        sv.end(),
        stop_regex,
        -1
    );
    std::regex_token_iterator<std::string_view::iterator> end;

    std::vector<std::string> stops_names;
    while (i != end)
        stops_names.push_back(*i++);
    return stops_names;
}

void FillFromInput(TransportCatalogue& transport_catalogue) {
    const int inputs_count = ReadLineWithNumber();
    for (int i = 0; i < inputs_count; ++i) {
        std::string line = ReadLine();
        if (std::cin) {
            static std::regex const stop_matcher(
                R"^(Stop\s*([^,]*):\s*(\d+.\d+),\s*(\d+.\d+))^"
            );
            static std::regex const bus_matcher(R"^(Bus\s*(\d+):\s*(.*?))^");
            std::smatch capture;

            if (std::regex_match(line, capture, stop_matcher)) {
                Stop stop;
                stop.name = capture[1];
                stop.coords.lat = std::stod(capture[2]);
                stop.coords.lng = std::stod(capture[3]);

                transport_catalogue.AddStop(stop);
            } else if (std::regex_match(line, capture, bus_matcher)) {
                Bus bus;
                bus.no = std::stoi(capture[1]);
                bus.stops_names = SplitIntoStopsNames(capture.str(2));

                transport_catalogue.AddBus(bus);
            } else {
                std::cin.setstate(std::ios_base::failbit);
            }
        }
    }
}