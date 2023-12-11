#ifndef CAMERA_H
#define CAMERA_H

#include "utility.h"
#include "scene.h"
#include "material.h"

#include <iostream>

class camera {
public:
    camera(point3 lookfrom = point3(0.0,0.0,0.0),
           point3 lookat = point3(0.0,0.0,-1.0),
           point3 up = point3(0.0,1.0,0.0),
           int image_width = 400,
           double aspect_ratio = 16.0 / 9.0,
           double fov = 90.0,
           double defocus_angle = 0.0,
           double focus_dist = 10.0,
           int super_sample = 10,
           int max_depth = 10) :
    lookfrom(lookfrom),
    lookat(lookat),
    up(up),
    image_width(image_width),
    aspect_ratio(aspect_ratio),
    fov(fov),
    defocus_angle(defocus_angle),
    focus_dist(focus_dist),
    super_sample(super_sample),
    max_depth(max_depth){
        initialize();
    }

    void initialize() {
        camera_pos = lookfrom;

        w = unit_vector(lookat - lookfrom);
        u = unit_vector(cross(w, up));
        v = cross(u, w);

        image_height = static_cast<int>(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;
        // in case the expected aspect_ratio is not consistent with the real ratio
        double viewport_height = 2 * tan(degrees_to_radians(fov / 2)) * focus_dist;
        double viewport_width = static_cast<double>(image_width) / image_height * viewport_height;
        vec3 viewport_u = u * viewport_width;
        vec3 viewport_v = -v * viewport_height;
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;
        point3 left_top = camera_pos + (focus_dist * w) - viewport_u / 2 - viewport_v / 2;
        left_top_pixel_center = left_top + 0.5 * (pixel_delta_v + pixel_delta_u);

        // Calculate the camera defocus disk basis vectors.
        double defocus_radius = focus_dist * tan(degrees_to_radians(defocus_angle / 2));
        defocus_disk_u = u * defocus_radius;
        defocus_disk_v = v * defocus_radius;
    }

    void render(const scene& world) {
        std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

        for (int j = 0; j < image_height; j ++){
            std::clog << "\rScanlines remaining:" << (image_height - j) << ' ' << std::flush;
            for (int i = 0; i < image_width; i ++){
                color pixel_color (0.0, 0.0, 0.0);
                for (int sample = 0; sample < super_sample; sample++){
                    ray ray = cast_ray(i, j);
                    pixel_color += ray_color(ray, max_depth, world);
                }

                write_color(std::cout, pixel_color, super_sample);
            }
        }
        std::clog <<"\r Done.                       \n";
    }
    point3 lookfrom;
    point3 lookat;
    vec3 up;
    double aspect_ratio; // ratio of image width over height
    double fov; // field of view
    int super_sample;  // number of rays cast for each pixel
    int max_depth;  // maximum number of ray bounces

    double defocus_angle; // Variation angle of rays through each pixel
    double focus_dist; // Distance from camera lookfrom point to plane of perfect focus
private:
    // camera center at (0,0,0), view direction is (0,0,-1)
    point3 camera_pos;
    int image_width;
    int image_height;

    // x, y, z axis
    vec3 u, v, w;

    // u horizontal, v vertical
    vec3 pixel_delta_u;
    vec3 pixel_delta_v;
    point3 left_top_pixel_center;
    vec3 defocus_disk_u; // Defocus disk horizontal radius
    vec3 defocus_disk_v; // Defocus disk vertical radius

    color ray_color(const ray& r, int depth, const scene& world) const {
        intersection intersect;
        if (depth <= 0)
            return {0.0, 0.0, 0.0};

        if (world.intersect(r, interval(0.001, infinity), intersect)){
            ray scattered_ray;
            color attenuation;
            if (intersect.mat->scatter(r, intersect, attenuation, scattered_ray)){
                return attenuation * ray_color(scattered_ray, depth - 1, world);
            }
            return {0.0, 0.0, 0.0};
        }
        vec3 unit_direction = unit_vector(r.direction());
        auto a = 0.5*(unit_direction[1] + 1.0);
        return (1.0-a)*color(1.0, 1.0, 1.0) + a*color(0.5, 0.7, 1.0);
    }

    ray cast_ray(int i, int j){
        point3 pixel = left_top_pixel_center + (i * pixel_delta_u) + (j * pixel_delta_v) + pixel_sample_square();
        point3 ray_origin = (defocus_angle <= 0)? camera_pos : defocus_disk_sample();
        return {ray_origin, pixel - ray_origin};
    }

    vec3 pixel_sample_square() const {
        // Returns a random point in the square surrounding a pixel at the origin.
        double px = -0.5 + random_double();
        double py = -0.5 + random_double();
        return (px * pixel_delta_u) + (py * pixel_delta_v);
    }

    point3 defocus_disk_sample() const {
//        double u_rand = random_double(-1.0, 1.0);
//        double limit = sqrt(1.0 - u_rand * u_rand);
//        double v_rand = random_double(-limit, limit);
//        return camera_pos + u_rand * defocus_disk_u + v_rand * defocus_disk_v;
        vec3 p = random_in_unit_disk();
        return camera_pos + p[0] * defocus_disk_u + p[1] * defocus_disk_v;
    }
};


#endif //CAMERA_H
