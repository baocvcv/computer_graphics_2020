#ifndef COMMON_HPP_
#define COMMON_HPP_

#include <bits/stdc++.h>

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"
#endif

const double eps = 1e-6;

enum class MaterialType {
    DIFFUSE,
    SPECULAR,
    REFRACTIVE
};

inline double clamp(double x) { return x < 0 ? 0 : x > 1 ? 1 : x; }

inline int gamma_trans(double x) { return 255 * pow(clamp(x), 1. / 2.2) + .5; }

inline double square(double x) { return x * x; }


#endif