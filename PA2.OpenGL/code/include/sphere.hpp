#ifndef SPHERE_H
#define SPHERE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>
#include <glut.h>

class Sphere : public Object3D {
public:
    Sphere() {
        // unit ball at the center
        center = Vector3f::ZERO;
        radius = 1.0;
    }

    Sphere(const Vector3f &_center, float _radius, Material *_material) :
            Object3D(_material), center(_center), radius(_radius) {}

    ~Sphere() override = default;

    bool intersect(const Ray &r, Hit &h, float tmin) override {
        // Calc dist from center to ray r
        Vector3f originToCent = center - r.getOrigin();
        double d1 = abs_f(Vector3f::dot(r.getDirection().normalized(), originToCent));
        double d2 = originToCent.squaredLength() - d1 * d1; // center to ray r
        // Cals intersection
        double interHalfLen = sqrt(radius*radius - d2);
        double t = d1 - interHalfLen;
        if (t < tmin) return false;
        else {
            Vector3f normal = r.pointAtParameter(t) - center;
            h.set(t, material, normal.normalized());
            return true;
        }
    }

    void drawGL() override {
        Object3D::drawGL();
        glMatrixMode(GL_MODELVIEW); glPushMatrix();
        glTranslatef(center.x(), center.y(), center.z());
        glutSolidSphere(radius, 80, 80);
        glPopMatrix();
    }

protected:
    Vector3f center;
    float radius;

};


#endif
