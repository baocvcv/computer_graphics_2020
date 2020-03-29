#ifndef MATERIAL_H
#define MATERIAL_H

#include <cassert>
#include <vecmath.h>

#include "ray.hpp"
#include "hit.hpp"
#include <iostream>

class Material {
public:

    explicit Material(const Vector3f &d_color, const Vector3f &s_color = Vector3f::ZERO, float s = 0) :
            diffuseColor(d_color), specularColor(s_color), shininess(s) {

    }

    virtual ~Material() = default;

    virtual Vector3f getDiffuseColor() const {
        return diffuseColor;
    }

    // need to make sure all the vectors are normalized
    Vector3f Shade(const Ray &ray, const Hit &hit,
                   const Vector3f &dirToLight, const Vector3f &lightColor) {
        Vector3f shaded = Vector3f::ZERO;

        float t1 = Vector3f::dot(dirToLight, hit.getNormal()); // Lx . N
        Vector3f Rx = hit.getNormal() * 2 * t1 - dirToLight;
        t1 = ReLU(t1);
        float t2 = ReLU(-Vector3f::dot(ray.getDirection(), Rx));
        t2 = pow(t2, shininess);
        shaded = lightColor * (diffuseColor * t1 + specularColor * t2);

        return shaded;
    }

    inline float ReLU(float x) { return (x > 0) ? x : .0; }

protected:
    Vector3f diffuseColor;
    Vector3f specularColor;
    float shininess;

};


#endif // MATERIAL_H
