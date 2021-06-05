#pragma once

#include <iostream>
#include <memory>
#include <stdexcept>

#include "json/json.h"
#include "transport_catalogue.h"

namespace transport {
namespace io {

static const int INDENT_SIZE = 2;

class JsonReader {
    using Request = std::unique_ptr<const json::Dict>;

public:
    JsonReader(std::istream& input)
            : requests_(json::Load(input).GetRoot().AsMap()) {
        ParseBuses();
        ParseStops();
        ParseStats();
    }

    inline const std::vector<Request>& GetBuses() const {
        return buses_;
    }

    inline const std::vector<Request>& GetStops() const {
        return stops_;
    }

    inline const std::vector<Request>& GetStats() const {
        return stats_;
    }

private:
    json::Dict requests_;
    std::vector<Request> buses_, stops_, stats_;

    void ParseBuses();

    void ParseStops();

    void ParseStats();
};

void Populate(TransportCatalogue& db, const JsonReader& reader);

void Search(const TransportCatalogue& db, const JsonReader& reader);

} // end namespace io
} // end namespace transport