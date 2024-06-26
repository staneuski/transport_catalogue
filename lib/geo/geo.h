#pragma once
#include <cmath>

namespace geo {

struct Coordinates {
    double lat;
    double lng;
};

inline double ComputeDistance(Coordinates from, Coordinates to) {
    using namespace std;

    static const double dr = M_PI/180.;
    return 6371000*acos(
        sin(from.lat*dr)*sin(to.lat*dr)
        + cos(from.lat*dr)*cos(to.lat*dr)*cos(abs(from.lng - to.lng)*dr)
    );
}

} // namespace geo