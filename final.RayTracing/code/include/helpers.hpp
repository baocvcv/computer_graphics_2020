#ifndef HELPERS_H_
#define HELPERS_H_

#include <cassert>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <string>
#include <Vector3f.h>
#include <vecmath.h>

class Material;

// Ray class mostly copied from Peter Shirley and Keith Morley
class Ray {
public:
    Vector3f origin;
    Vector3f dir;

    Ray() = delete;

    Ray(const Vector3f &orig, const Vector3f &_dir) : origin(orig), dir(_dir) {}

    Ray(const Ray &r) { origin = r.origin, dir = r.dir; }

    Vector3f pointAtParameter(float t) const { return origin + dir * t; }
};

inline std::ostream &operator<<(std::ostream &os, const Ray &r) {
    os << "Ray <" << r.getOrigin() << ", " << r.getDirection() << ">";
    return os;
}

// TODO: add u,v for texture mapping here
class Hit {
public:
    float t;
    Material *material;
    Vector3f normal;
    Vector2f uv;

    Hit() : material(nullptr), t(1e38) {}

    Hit(float _t, Material *m, const Vector3f &n, const Vector2f &uv_ = Vector2f::ZERO) :
        t(_t), material(m), normal(n), uv(uv_) {}

    Hit(const Hit &h) {
        t = h.t;
        material = h.material;
        normal = h.normal;
        uv = h.uv
    }

    void set(float _t, Material *_m, const Vector3f &n, const Vector2f &uv_ = Vector2f::ZERO) {
        t = _t;
        material = _m;
        normal = n;
        uv = uv_;
    }
};

inline std::ostream &operator<<(std::ostream &os, const Hit &h) {
    os << "Hit <" << h.t << ", " << h.normal << ">";
    return os;
}

class Material {
public:
    enum class MaterialType {
        DIFFUSE,
        SPECULAR,
        REFRACT
    };
    MaterialType type;
    Vector3f color;
    Vector3f emission;
    float n_material; // refraction index

    bool has_texture;
    std::string texture_file;

    Material(
        MaterialType type_,
        const Vector3f &color_ = Vector3f::ZERO,
        const Vector3f &emission_ = Vector3f::ZERO,
        float n = 1.) :
            type(type_), color(color_), emission(emission_), n_material(n),
            has_texture(false) {}

    Material(
        MaterialType type_,
        const std::string& texture,
        const Vector3f &color_ = Vector3f::ZERO,
        const Vector3f &emission_ = Vector3f::ZERO,
        float n = 1.) :
            type(type_), color(color_), emission(emission_), n_material(n),
            has_texture(true),texture_file(texture) {}

    virtual ~Material() = default;

    Vector3f Shade(const Ray &ray, const Hit &hit,
                   const Vector3f &dirToLight, const Vector3f &lightColor) {
        Vector3f shaded = Vector3f::ZERO;

        // float t1 = Vector3f::dot(dirToLight, hit.normal); // Lx . N
        // Vector3f Rx = hit.normal * 2 * t1 - dirToLight;
        // t1 = ReLU(t1);
        // float t2 = ReLU(-Vector3f::dot(ray.getDirection(), Rx));
        // t2 = pow(t2, shininess);
        // shaded = lightColor * (diffuseColor * t1 + specularColor * t2);

        return shaded;
    }

    inline float ReLU(float x) { return (x > 0) ? x : .0; }

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
        Vector3f n = hit.normal, rd = ray.dir;
        Vector3f d = rd - n * 2 * Vector3f::dot(n, rd);
        return Ray(x, d);
    }

    // TODO: currently assume every object is surrounded by air, can probably improve to object surrounded by object?
    // return <reflect, refract>
    std::pair<std::pair<Ray, double>, std::pair<Ray, double>> refractiveRay(
            const Ray &ray, const Hit &hit, unsigned short *Xi) {

        double n_air = 1;
        //TODO: understande the math here
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

        Vector3f refract_d = norm * (sqrt(1 - sin_theta*sin_theta / (n*n)) - cos_theta/n) + ray.dir / n;
        Ray refract(ray.pointAtParameter(hit.t), refract_d);
        double refract_i = r0 + (1. - r0) * pow((1. - cos_theta), 5);
        double reflect_i = 1. - refract_i;
        return { {reflect, reflect_i}, {refract, refract_i} };
    }
};

#endif