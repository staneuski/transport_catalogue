#pragma once

#include <database.pb.h>
#include <catalogue.pb.h>
#include <domain.pb.h>
#include <geo.pb.h>

#include "request_handler.h"

namespace transport {
namespace io {

class Bufferiser {
public:
    Bufferiser(const RequestHandler& request_handler)
        : request_handler_(request_handler) {
    }

    void Serialize(std::ostream& out) const;

    void Deserialize(std::istream& in);

private:
    const RequestHandler& request_handler_;

    static pb::domain::Stop Convert(const domain::Stop& stop);

    static pb::domain::AdjacentStops Convert(
        const Catalogue::AdjacentStops adjacent_stops,
        const int distance
    );

    pb::domain::Bus Convert(const domain::Bus& bus) const;

    domain::Bus Convert(const pb::domain::Bus& bus) const;
};

} // namespace io
} // namespace transport
