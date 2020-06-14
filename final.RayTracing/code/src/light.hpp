#ifndef LIGHT_H
#define LIGHT_H

#include "object3d.hpp"
#include <Vector3f.h>

class Light {
public:
    Light() = default;

    virtual ~Light() = default;

    virtual void getIllumination(const Vec3 &p, Vec3 &dir, Vec3 &col) const = 0;
};


class DirectionalLight : public Light {
public:
    DirectionalLight() = delete;

    DirectionalLight(const Vec3 &d, const Vec3 &c) {
        direction = d.normalized();
        color = c;
    }

    ~DirectionalLight() override = default;

    ///@param p unsed in this function
    ///@param distanceToLight not well defined because it's not a point light
    void getIllumination(const Vec3 &p, Vec3 &dir, Vec3 &col) const override {
        // the direction to the light is the opposite of the
        // direction of the directional light source
        dir = -direction;
        col = color;
    }

private:

    Vec3 direction;
    Vec3 color;

};

class PointLight : public Light {
public:
    PointLight() = delete;

    PointLight(const Vec3 &p, const Vec3 &c) {
        position = p;
        color = c;
    }

    ~PointLight() override = default;

    void getIllumination(const Vec3 &p, Vec3 &dir, Vec3 &col) const override {
        // the direction to the light is the opposite of the
        // direction of the directional light source
        dir = (position - p);
        dir = dir / dir.len();
        col = color;
    }

private:

    Vec3 position;
    Vec3 color;

};

#endif // LIGHT_H
