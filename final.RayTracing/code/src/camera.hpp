#ifndef CAMERA_H
#define CAMERA_H

#include "common.hpp"
#include "helpers.hpp"
#include "image.hpp"
#include "group.hpp"
#include "vec.hpp"
#include "mat44.hpp"

struct Camera {
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
        this->horizontal = this->direction.cross(up); // Vec3::cross(this->direction, up);
        this->up = this->horizontal.cross(this->direction); //Vec3::cross(this->horizontal, this->direction);
        this->width = imgW;
        this->height = imgH;
    }

    // Generate rays for each screen-space coordinate
    virtual Ray generateRay(const Vec3 &point, unsigned short* Xi) = 0;
    // virtual void renderFrame(const SceneParser& sp, Image& outImg, int n_samples) = 0;
    virtual ~Camera() = default;
};

// TODO: Implement Perspective camera
// You can add new functions or variables whenever needed.
struct PerspectiveCamera : public Camera {
    double distToCanvas;
    Vec3 bottomLeft;

    PerspectiveCamera(const Vec3 &center, const Vec3 &direction,
            const Vec3 &up, int imgW, int imgH, double angle) : Camera(center, direction, up, imgW, imgH) {
        // angle is in radian.
        distToCanvas = imgW / 2.0 / tan(angle / 2);
        bottomLeft = this->center + this->direction * distToCanvas
            - this->horizontal * width / 2 - this->up * height / 2;
    }

    Ray generateRay(const Vec3 &point, unsigned short* Xi) override {
        // Vec3 d_rc(point.x-width/2+1, point.y-height/2+1, distToCanvas);
        // d_rc.normalize();
        // Mat44 R(horizontal, up, direction);
        // return Ray(center, R.mult(d_rc));
        auto dir = bottomLeft + horizontal * point.x + up * point.y - center;
        return Ray(center, dir.normalized());
    }
};

struct DoFCamera : public PerspectiveCamera {
    double focus_to_canvas_ratio;
    double lens_radius;

    DoFCamera(
        const Vec3& center,
        const Vec3& direction,
        const Vec3& up,
        int imgW,
        int imgH,
        double angle,
        double _aperture,
        double focusDist
    ): PerspectiveCamera(center, direction, up, imgW, imgH, angle), lens_radius(_aperture/2) {
        focus_to_canvas_ratio = focusDist / distToCanvas;
    }

    Ray generateRay(const Vec3& point, unsigned short* Xi) override {
        auto dir_to_canvas = bottomLeft + horizontal * point.x + up * point.y - center;
        auto dir_to_focus_plane = dir_to_canvas * focus_to_canvas_ratio;
        auto point_on_focus_plane = center + dir_to_focus_plane;

        auto rd = Vec3::random_in_unit_disk(Xi) * lens_radius;
        auto new_center = center + horizontal * rd.x + up * rd.y;
        return Ray(new_center, (point_on_focus_plane - new_center).normalized());
    }
};
#endif //CAMERA_H
