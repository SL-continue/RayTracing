#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include <utility>

#include "ray.h"
#include "intersection.h"


class primitive{
public:
    explicit primitive(const point3& c, std::shared_ptr<material> mat = nullptr): center(c), mat(std::move(mat)){}
    virtual bool intersect(const ray& r, interval ray_t, intersection& inter) const = 0;
    point3 local_center() { return center; }

protected:
    point3 center;
    std::shared_ptr<material> mat;
};

class sphere : public primitive {
public:
    sphere(const point3& c, const double r, std::shared_ptr<material> m = nullptr): primitive(c, std::move(m)), radius(r){}

    bool intersect(const ray& r, interval ray_t, intersection& inter) const override {
        vec3 oc = r.origin() - center;
        double a = r.direction().length_squared();
        double half_b = dot(oc, r.direction());
        double c = oc.length_squared() - radius*radius;
        double discriminant = half_b * half_b - a*c;
        if (discriminant < 0) return false;

        // find the closest root
        double sqrt_d = sqrt(discriminant);
        double root = (-half_b - sqrt_d) / a;
        if (! ray_t.surrounds(root)){
            root = (-half_b + sqrt_d) / a;
            if (! ray_t.surrounds(root)){
                return false;
            }
        }
        inter.t = root;
        inter.point = r.at(root);
        inter.mat = mat;
        vec3 outward_normal = (inter.point - center) / radius;
        inter.set_face_normal(r, outward_normal);
        return true;
    }
private:
    double radius;
};

class plane : public primitive {
public:
    plane(const point3& c, const double l, std::shared_ptr<material> m = nullptr): primitive(c, m), width(l), length(l), u(unit_vector(vec3(1.0, 0.0, 0.0))), normal(unit_vector(vec3(0.0, 1.0, 0.0))) { v = cross(normal, u); }
    plane(const point3& c, const double l, const vec3& n = vec3(0.0, 1.0, 0.0), const vec3& u = vec3(1.0, 0.0, 0.0), std::shared_ptr<material> m = nullptr): primitive(c, m), width(l), length(l), u(unit_vector(u)), normal(unit_vector((n))) { v = cross(normal, u); }
    plane(const point3& c, const double w, const double l, const vec3& n = vec3(0.0, 1.0, 0.0), const vec3& u = vec3(1.0, 0.0, 0.0), std::shared_ptr<material> m = nullptr): primitive(c, m), width(w), length(l), u(unit_vector(u)), normal(unit_vector(n)) { v = cross(normal, u); }

    bool intersect(const ray& r, interval ray_t, intersection& inter) const override {
        if (fabs(dot(r.direction(), normal)) < 0.001)
            return false;
        double t = dot((center - r.origin()), normal) / dot(r.direction(), normal);
        if (!ray_t.contains(t))
            return false;
        point3 p = r.at(t);
        vec3 gap = p - center;
        double u_distance = (gap[0] * v[1] - gap[1] * v[0]) / (u[0] * v[1] - u[1] * v[0]);
        double v_distance = (gap[0] - u_distance * u[0]) / v[0];
        if (width < u_distance || length < v_distance)
            return false;
        inter.point = p;
        inter.mat = mat;
        inter.t = t;
        inter.set_face_normal(r, normal);
        return true;
    }

private:
    double width;
    double length;
    vec3 normal;
    vec3 u;
    vec3 v;
};

#endif //PRIMITIVE_H
