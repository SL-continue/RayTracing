#ifndef INTERSECTION_H
#define INTERSECTION_H

#include "vec3.h"

class material;

class intersection {
public:
    double t;
    point3 point;
    vec3 normal;
    vec3 color;
    std::shared_ptr<material> mat;
    bool front;

    void set_face_normal(const ray& r, const vec3& outward_normal){
        // outward_normal is assumed to have unit length
        front = dot(r.direction(), outward_normal) < 0;
        normal = front ? outward_normal : -outward_normal;
    }
};

#endif //INTERSECTION_H
