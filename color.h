#ifndef COLOR_H
#define COLOR_H

#include <iostream>

class vec3;

using color = vec3;

// apply gamma correction
inline double linear_to_gamma(double linear_component){
    return sqrt(linear_component);
}

void write_color(std::ostream &out, color pixel_color, int super_sample, bool use_gamma = true) {
    double r = pixel_color[0];
    double g = pixel_color[1];
    double b = pixel_color[2];

    double scale = 1.0 / super_sample;
    r *= scale;
    g *= scale;
    b *= scale;

    // apply gamma correction
    r = (use_gamma) ? linear_to_gamma(r) : r;
    g = (use_gamma) ? linear_to_gamma(g) : g;
    b = (use_gamma) ? linear_to_gamma(b) : b;

    // Write the translated [0,255] value of each color component.
    static const interval intensity(0.0, 255.0);
    out << static_cast<int>(intensity.clamp(256 * r)) << ' '
        << static_cast<int>(intensity.clamp(256 * g)) << ' '
        << static_cast<int>(intensity.clamp(256 * b)) << '\n';
}

#endif //COLOR_H
