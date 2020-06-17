#ifndef COMMON_HPP_
#define COMMON_HPP_

#include <bits/stdc++.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#define M_PI
#endif

const double eps = 1e-7;

enum class MaterialType {
    DIFFUSE,
    SPECULAR,
    REFRACTIVE
};

inline double clamp(double x) { return x < 0 ? 0 : x > 1 ? 1 : x; }

inline unsigned char gamma_trans(double x) {
    return 255 * pow(clamp(x), 1. / 2.2) + .5;
}

inline double square(double x) { return x * x; }


#endif