#ifndef CAMERA_H
#define CAMERA_H

#include "common.hpp"
#include "helpers.hpp"
#include "light.hpp"
#include "image.hpp"
#include "group.hpp"
#include "vec.hpp"
#include "mat44.hpp"

class Camera {
public:
    // Extrinsic parameters
    Vec3 center;
    Vec3 direction;
    Vec3 up;
    Vec3 horizontal;
    // Intrinsic parameters
    int width;
    int height;

    Camera(const Vec3 &center, const Vec3 &direction, const Vec3 &up, int imgW, int imgH) {
        this->center = center;
        this->direction = direction.normalized();
        this->horizontal = this->direction % up; // Vec3::cross(this->direction, up);
        this->up = this->horizontal % this->direction; //Vec3::cross(this->horizontal, this->direction);
        this->width = imgW;
        this->height = imgH;
    }

    // Generate rays for each screen-space coordinate
    virtual Ray generateRay(const Vec3 &point) = 0;
    // virtual void renderFrame(const SceneParser& sp, Image& outImg, int n_samples) = 0;
    virtual ~Camera() = default;
};

// TODO: Implement Perspective camera
// You can add new functions or variables whenever needed.
class PerspectiveCamera : public Camera {
public:
    double distToCanvas;

    PerspectiveCamera(const Vec3 &center, const Vec3 &direction,
            const Vec3 &up, int imgW, int imgH, float angle) : Camera(center, direction, up, imgW, imgH) {
        // angle is in radian.
        distToCanvas = imgW / 2.0 / tan(angle / 2);
    }

    Ray generateRay(const Vec3 &point) override {
        Vec3 d_rc(point.x-width/2+1, point.y-height/2+1, distToCanvas);
        d_rc.normalize();
        // Matrix3f R(horizontal, up, direction);
        Mat44 R(horizontal, up, direction);
        return Ray(center, R.mult(d_rc));
    }
};

#endif //CAMERA_H
