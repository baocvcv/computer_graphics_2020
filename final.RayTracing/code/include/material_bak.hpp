#ifndef MATERIAL_H
#define MATERIAL_H

#include "helpers.hpp"

#include <cassert>
#include <cstdlib>
#include <cmath>
#include <vecmath.h>
#include <iostream>
#include <memory>

class Material {
public:

    explicit Material(const Vector3f &d_color, const Vector3f &s_color = Vector3f::ZERO, float s = 0) :
            diffuseColor(d_color), specularColor(s_color), shininess(s) {

    }

    virtual ~Material() = default;

    virtual Vector3f getDiffuseColor() const {
        return diffuseColor;
    }

    Vector3f Shade(const Ray &ray, const Hit &hit,
                   const Vector3f &dirToLight, const Vector3f &lightColor) {
        Vector3f shaded = Vector3f::ZERO;

        float t1 = Vector3f::dot(dirToLight, hit.normal); // Lx . N
        Vector3f Rx = hit.normal * 2 * t1 - dirToLight;
        t1 = ReLU(t1);
        float t2 = ReLU(-Vector3f::dot(ray.getDirection(), Rx));
        t2 = pow(t2, shininess);
        shaded = lightColor * (diffuseColor * t1 + specularColor * t2);

        return shaded;
    }

    inline float ReLU(float x) { return (x > 0) ? x : .0; }

    // TODO: Invariant: needs to make sure that normal always at a >90 angle relative to incoming ray
    Ray diffuseRay(const Ray &ray, const Hit &hit, unsigned short *Xi) {
        // Ideal DIFFUSE reflection
        double r1 = 2 * M_PI * erand48(Xi); // angle
        double r2 = erand48(Xi), r2s = sqrt(r2);
        Vector3f w = hit.normal;
        // u, v are vectors on the tangent plane at x

        Vector3f u = (fabs(w.x()) > .1 ? Vector3f(0, 1, 0)
                                   : Vector3f::cross(Vector3f(1, 0, 0), w)).normalized();
        Vector3f v = Vector3f::cross(w, u);
        // TODO: what is this?
        Vector3f d = (u * cos(r1) * r2s + v * sin(r1) * r2s + w * sqrt(1 - r2)).normalized();
        Vector3f x = ray.pointAtParameter(hit.t);
        return Ray(x, d);
    }

    Ray specularRay(const Ray &ray, const Hit &hit) {
        Vector3f x = ray.pointAtParameter(hit.t);
        Vector3f n = hit.normal, rd = ray.getDirection();
        Vector3f d = rd - n * 2 * Vector3f::dot(n, rd);
        return Ray(x, d);
    }

    // TODO: currently assume every object is surrounded by air, can probably improve to object surrounded by object?
    // return <reflect, refract>
    std::pair<std::pair<Ray, double>, std::pair<Ray, double>> refractiveRay(
            const Ray &ray, const Hit &hit, unsigned short *Xi) {

        double n_air = 1;
        double r0 = (n_air - n_material) * (n_air - n_material) / (n_air + n_material) * (n_air + n_material);
        Ray reflect = specularRay(ray, hit);
        double cos_theta = Vector3f::dot(ray.dir, hit.normal); // r.d.dot(nl)
        double sin_theta = sqrt(1 - cos_theta * cos_theta);
        double n;
        Vector3f norm = hit.normal;
        if (cos_theta < 0) { // from inside
            n = n_material / n_air;
            cos_theta = -cos_theta;
            norm = (-1) * norm;
        } else { // from outside
            n = n_air / n_material;
            if (sin_theta >= n) {
                return { {reflect, 1.}, {reflect, .0} };
            }
        }

        Vector3f refract_d = norm * (sqrt(1 - sin_theta*sin_theta / (n*n)) - cos_theta/n) + r.d / n;
        Ray refract(r.pointAtParameter(hit.t), refract_d);
        double refract_i = r0 + (1. - r0) * pow((1. - cos_theta), 5);
        double reflect_i = 1. - refract_i;
        return { {reflect, reflect_i}, {refract, refract_i} };
    }

protected:
    Vector3f diffuseColor;
    Vector3f specularColor;
    float shininess;
    float n_material; // refraction index
};


#endif // MATERIAL_H
