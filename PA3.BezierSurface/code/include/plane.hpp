#ifndef PLANE_H
#define PLANE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>

class Plane : public Object3D {
public:
    Plane() {}

    Plane(const Vector3f &normal, float d, Material *m) : Object3D(m) {
        this->d = d;
        this->norm = normal;
    }

    ~Plane() override = default;

    bool intersect(const Ray &r, Hit &h, float tmin) override {
        // ray r is parallel with the plane
        if (abs_f(Vector3f::dot(r.getDirection(), norm)) < 1e-8)
            return false;

        // not parallel
        float t = Plane::solve(r, norm, d);
        if (t < tmin) // too close or wrong direction
            return false;
        else {
            h.set(t, material, norm);
            return true;
        }
    }
    
    static float solve(const Ray &r, const Vector3f &normal, float d) {
        float s = d - Vector3f::dot(normal, r.getOrigin());
        float t = s / Vector3f::dot(normal, r.getDirection());
        return t;
    }


    void drawGL() override {
        Object3D::drawGL();
        Vector3f xAxis = Vector3f::RIGHT;
        Vector3f yAxis = Vector3f::cross(norm, xAxis);
        xAxis = Vector3f::cross(yAxis, norm);
        const float planeSize = 10.0;
        glBegin(GL_TRIANGLES);
        glNormal3fv(norm);
        glVertex3fv(d * norm + planeSize * xAxis + planeSize * yAxis);
        glVertex3fv(d * norm - planeSize * xAxis - planeSize * yAxis);
        glVertex3fv(d * norm + planeSize * xAxis - planeSize * yAxis);
        glNormal3fv(norm);
        glVertex3fv(d * norm + planeSize * xAxis + planeSize * yAxis);
        glVertex3fv(d * norm - planeSize * xAxis + planeSize * yAxis);
        glVertex3fv(d * norm - planeSize * xAxis - planeSize * yAxis);
        glEnd();
    }

protected:
    Vector3f norm;
    float d;

};

#endif //PLANE_H
		

