#ifndef GROUP_H
#define GROUP_H

#include "object3d.hpp"
#include "helpers.hpp"

#include <iostream>
#include <vector>

class Group : public Object3D {
public:
    std::vector<Object3D*> objects;

    Group() {}

    ~Group() override {
        for (auto obj: objects) {
            // delete obj;
        }
    }

    bool intersect(const Ray &r, Hit &h, float tmin) override {
        // intersect every object in objects
        // find the closest and return
        bool hasIntersect = false;
        Hit h_tmp;
        for (auto obj: objects) {
            if (obj->intersect(r, h_tmp, tmin) && h_tmp.t < h.t) {
                h = h_tmp;
                hasIntersect = true;
            }
        }
        return hasIntersect;
    }

    void addObject(Object3D *obj) { objects.push_back(obj); }

   int getGroupSize() { return objects.size(); }
};

#endif
	
