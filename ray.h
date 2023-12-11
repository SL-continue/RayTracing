#ifndef RAY_H
#define RAY_H

#include "vec3.h"

class ray {
public:
    ray() {}
    ray(const point3& origin, const vec3& direction): ori(origin), dir(direction), tm(0) {}

    ray(const point3& origin, const vec3& direction, double time = 0.0)
    : ori(origin), dir(direction), tm(time) {}

    point3 origin() const { return ori; }
    vec3 direction() const { return dir; }
    double time() const { return tm; }

    point3 at(double t) const { return ori + dir * t; }

private:
    point3 ori; // origin
    vec3 dir;   // direction, not normalized
    double tm;  // time
};

#endif //RAY_H
