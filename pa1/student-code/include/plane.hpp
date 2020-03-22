#ifndef PLANE_H
#define PLANE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>

#define ABS(x) ((x < 0) ? (-x) : (x))

// TODO: Implement Plane representing an infinite plane
// function: ax+by+cz=d
// choose your representation , add more fields and fill in the functions

class Plane : public Object3D {
public:
    Plane(): Object3D() {}

    Plane(const Vector3f &normal, float d, Material *m) : Object3D(m) {
        this->d = d / normal.length();
        this->normal = normal.normalized();
    }

    ~Plane() override = default;

    bool intersect(const Ray &r, Hit &h, float tmin) override {
        // ray r is parallel with the plane
        if (ABS(Vector3f::dot(r.getDirection(), normal)) < 1e-6)
            return false;

        // not parallel
        float t = Plane::solve(r, normal, d);
        if (t < tmin || t > h.getT())
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
