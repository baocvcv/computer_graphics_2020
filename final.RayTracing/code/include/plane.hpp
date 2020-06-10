#ifndef PLANE_H
#define PLANE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>

// Implement Plane representing an infinite plane
// function: ax+by+cz=d
// choose your representation , add more fields and fill in the functions
class Plane : public Object3D {
public:
    Plane(): Object3D() {}

    Plane(const Vector3f &normal, float d, Material *m) : Object3D(m) {
        this->d = d;
        this->normal = normal;
    }

    ~Plane() override = default;

    bool intersect(const Ray &r, Hit &h, float tmin) override {
        // ray r is parallel with the plane
        if (abs_f(Vector3f::dot(r.getDirection(), normal)) < 1e-8)
            return false;

        // not parallel
        float t = Plane::solve(r, normal, d);
        if (t < tmin) // too close or wrong direction
            return false;
        else {
            h.set(t, material, normal);
            return true;
        }
    }

    static float solve(const Ray &r, const Vector3f &normal, float d) {
        float s = d - Vector3f::dot(normal, r.getOrigin());
        float t = s / Vector3f::dot(normal, r.getDirection());
        return t;
    }

protected:
    Vector3f normal;
    float d;

};

#endif //PLANE_H
