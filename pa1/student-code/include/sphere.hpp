#ifndef SPHERE_H
#define SPHERE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>

// TODO: Implement functions and add more fields as necessary

class Sphere : public Object3D {
public:
    Sphere(): Object3D() {
        // unit ball at the center
        center = Vector3f::ZERO;
        radius = 1.0;
    }

    Sphere(const Vector3f &_center, float _radius, Material *material) :
        Object3D(material), center(_center), radius(_radius) {}

    ~Sphere() override = default;

    bool intersect(const Ray &r, Hit &h, float tmin) override {
        // Calculate distance from center to ray r
        Vector3f oriToP = center - r.getOrigin();
        float d1 = abs(Vector3f::dot(r.getDirection(), oriToP));
        float d2 = oriToP.squaredLength() - d1 * d1;
        if (d2 >= radius * radius) return false;

        // intersects
        float intersectionHalfLen = sqrt(radius * radius - d2);
        float t = d1 - intersectionHalfLen;
        if (t < tmin || t > h.getT())
            return false;
        else {
            Vector3f normal = r.pointAtParameter(t) - center;
            h.set(t, material, normal.normalized());
            return true;
        }
    }

protected:
    Vector3f center;
    float radius;

};


#endif
