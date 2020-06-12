#ifndef CAMERA_H
#define CAMERA_H

#include "helpers.hpp"
#include "light.hpp"
#include "image.hpp"
#include "group.hpp"
#include "scene_parser.hpp"

#include <vecmath.h>
#include <float.h>
#include <cmath>

class Camera {
public:
    // Extrinsic parameters
    Vector3f center;
    Vector3f direction;
    Vector3f up;
    Vector3f horizontal;
    // Intrinsic parameters
    int width;
    int height;

    Camera(const Vector3f &center, const Vector3f &direction, const Vector3f &up, int imgW, int imgH) {
        this->center = center;
        this->direction = direction.normalized();
        this->horizontal = Vector3f::cross(this->direction, up);
        this->up = Vector3f::cross(this->horizontal, this->direction);
        this->width = imgW;
        this->height = imgH;
    }

    // Generate rays for each screen-space coordinate
    virtual Ray generateRay(const Vector2f &point) = 0;
    virtual void renderFrame(const SceneParser& sp, Image& outImg, int n_samples) = 0;
    virtual ~Camera() = default;
};

// TODO: Implement Perspective camera
// You can add new functions or variables whenever needed.
class PerspectiveCamera : public Camera {
public:
    double distToCanvas;

    PerspectiveCamera(const Vector3f &center, const Vector3f &direction,
            const Vector3f &up, int imgW, int imgH, float angle) : Camera(center, direction, up, imgW, imgH) {
        // angle is in radian.
        distToCanvas = imgW / 2.0 / tan(angle / 2);
    }

    Ray generateRay(const Vector2f &point) override {
        Vector3f d_rc(point.x()-width/2+1, point.y()-height/2+1, distToCanvas);
        d_rc.normalize();
        Matrix3f R(horizontal, up, direction);
        return Ray(center, R*d_rc);
    }

    void renderFrame(const SceneParser& sp, Image& outImg, int n_samples);
};

#endif //CAMERA_H
