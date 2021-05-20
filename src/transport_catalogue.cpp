#include "transport_catalogue.h"

void TransportCatalogue::AddStop(Stop&& stop) {
    stops_.push_back(std::move(stop));
    stop_names_[stops_.back().name] = &stops_.back();
}

void TransportCatalogue::AddBus(Bus&& bus) {
    buses_.push_back(std::move(bus));
    bus_numbers_[buses_.back().no] = &buses_.back();
}
