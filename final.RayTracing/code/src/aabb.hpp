#ifndef AABB_HPP_
#define AABB_HPP_

#include "common.hpp"
#include "vec.hpp"
#include "helpers.hpp"

float min(float a, float b, float c, float d) {
    float result = a;
    result = result < b ? result : b;
    result = result < c ? result : d;
    result = result < d ? result : d;
    return result;
}

float max(float a, float b, float c, float d) {
    float result = a;
    result = result > b ? result : b;
    result = result > c ? result : d;
    result = result > d ? result : d;
    return result;
}

struct AABB {
    Vec3 box_l;
    Vec3 box_h;

    AABB(): box_l(), box_h() {}
    AABB(const Vec3& a, const Vec3& b): box_l(min(a, b)), box_h(max(a, b)) {}
    AABB(const Vec3& a, const Vec3& b, const Vec3& c, const Vec3& d):
        box_l(min(min(a, b), min(c, d))), box_h(max(max(a, b), max(c, d))) {}
    AABB(const AABB& a, const AABB& b):
        box_l(min(a.box_l, b.box_l)), box_h(max(a.box_h, b.box_h)) {}
    AABB(const AABB& a, const AABB& b, const AABB& c, const AABB& d):
        box_l(min(min(a.box_l, b.box_l), min(c.box_l, d.box_l))),
        box_h(max(max(a.box_h, b.box_h), max(c.box_h, d.box_h))) {}

    bool intersect(const Ray& r, float& t_near_global, float& t_far_global) {
        float t_near = std::numeric_limits<float>::min();
        float t_far = std::numeric_limits<float>::max();
        Vec3 dir = r.dir;
        Vec3 o = r.origin;
        float arr[6];

        if (dir.x == 0) {
            if (o.x < box_l.x || o.x > box_h.x)
                return false;
            arr[0] = std::numeric_limits<float>::min();
            arr[1] = std::numeric_limits<float>::max();
        } else {
            arr[0] = (box_l.x - o.x) / dir.x;
            arr[1] = (box_h.x - o.x) / dir.x;
        }
        if (dir.y == 0) {
            if (o.y < box_l.y || o.y > box_h.y)
                return false;
            arr[2] = std::numeric_limits<float>::min();
            arr[3] = std::numeric_limits<float>::max();
        } else {
            arr[2] = (box_l.y - o.y) / dir.y;
            arr[3] = (box_h.y - o.y) / dir.y;
        }
        if (dir.z == 0) {
            if (o.z < box_l.z || o.z > box_h.z)
                return false;
            arr[4] = std::numeric_limits<float>::min();
            arr[5] = std::numeric_limits<float>::max();
        } else {
            arr[4] = (box_l.z - o.z) / dir.z;
            arr[5] = (box_h.z - o.z) / dir.z;
        }

        for (int i = 0; i < 6; i += 2) {
            if (arr[i] > arr[i+1]) {
                auto tmp = arr[i];
                arr[i] = arr[i+1];
                arr[i+1] = tmp;
            }
            if (t_near < arr[i]) t_near = arr[i];
            if (arr[i+1] < t_far) t_far = arr[i+1];
            if (t_near > t_far || t_far < 0) {
                return false;
            }
        }
        if (t_near != t_near) t_near = t_near * 1; //TODO: wtf???
        t_near_global = t_near, t_far_global = t_far;
        return true;
    }

    bool intersect(Vec3 triangle[3]) {
        //TODO: implement this for kd-tree

        return false;
    }
};

#endif