#ifndef MATERIAL_H
#define MATERIAL_H

#include "ray.h"
#include "intersection.h"

class material {
public:
    material(const color& c) : albedo(c) {}
    virtual ~material() = default;

    virtual bool scatter(
            const ray& r_in, const intersection& intersect, color& attenuation, ray& scattered) const = 0;

protected:
    color albedo;
};

class lambertian : public material {
public:
    lambertian(const color& a) : material(a) {}

    bool scatter(const ray& r_in, const intersection& intersect, color& attenuation, ray& scattered) const override{
        vec3 scatter_direction = intersect.normal + random_in_unit_sphere();
        if (scatter_direction.near_zero())
            scatter_direction = intersect.normal;

        scattered = ray(intersect.point, scatter_direction);
        attenuation = albedo;
        return true;
    }
};

class metal : public material {
public:
    metal(const color& c, double roughness) : material(c), roughness(roughness < 1 ? roughness : 1) {}

    bool scatter(const ray &r_in, const intersection &intersect, color &attenuation, ray &scattered) const override{
        vec3 direction = reflect(r_in.direction(), intersect.normal);
        scattered = ray(intersect.point, direction + roughness * random_unit_vector());
        attenuation = albedo;
        return (dot(scattered.direction(), intersect.normal) > 0);
    }
private:
    double roughness;
};

class dielectric : public material {
public:
    dielectric(const color& c, double ior) : material(c), ior(ior) {}

    dielectric(double ior) : ior(ior), material(color(1.0,1.0,1.0)) {}

    bool scatter(const ray &r_in, const intersection &intersect, color &attenuation, ray &scattered) const override{
        attenuation = albedo;
        vec3 in_direction = unit_vector(r_in.direction());
        double refraction_ratio = intersect.front ? (1.0 / ior) : ior;

        double cos_theta = fmin(dot(-in_direction, intersect.normal), 1.0);
        double sin_theta = sqrt(1.0 - cos_theta * cos_theta);
        vec3 direction;
        if (refraction_ratio * sin_theta > 1.0 || reflectance(cos_theta, refraction_ratio) > random_double()){
            direction = reflect(in_direction, intersect.normal);
        }else {
            direction = refract(in_direction, intersect.normal, refraction_ratio);
        }
        scattered = ray(intersect.point, direction);
        return true;
    }

private:
    static double reflectance(double cos_theta, double refraction_ratio) {
        // Schlick's approximation for reflectance
        double r0 = (1 - refraction_ratio) / (1 + refraction_ratio);
        r0 = r0 * r0;
        return r0 + (1-r0) * pow((1-cos_theta), 5);
    }

    double ior;
};

#endif //MATERIAL_H
