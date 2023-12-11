#ifndef UTILITY_H
#define UTILITY_H

#include <cmath>
#include <random>

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.14159265358979323846;

inline double degrees_to_radians(double degrees) {
    return degrees * pi / 180.0;
}

inline double random_double(){
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    static std::mt19937 generator;
    return distribution(generator);
}

inline double random_double(double min, double max){
    static std::uniform_real_distribution<double> distribution(min, max);
    static std::mt19937 generator;
    return distribution(generator);
}

#include "interval.h"
#include "ray.h"
#include "color.h"

#endif //UTILITY_H
