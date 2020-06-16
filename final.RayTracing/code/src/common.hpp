#ifndef COMMON_HPP_
#define COMMON_HPP_

#include <bits/stdc++.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#define M_PI
#endif

const float eps = 1e-6;

enum class MaterialType {
    DIFFUSE,
    SPECULAR,
    REFRACTIVE
};

inline float clamp(float x) { return x < 0 ? 0 : x > 1 ? 1 : x; }

inline unsigned char gamma_trans(float x) {
    return 255 * pow(clamp(x), 1. / 2.2) + .5;
}

inline float square(float x) { return x * x; }


#endif