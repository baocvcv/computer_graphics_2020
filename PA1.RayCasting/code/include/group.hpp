#ifndef GROUP_H
#define GROUP_H


#include "object3d.hpp"
#include "ray.hpp"
#include "hit.hpp"
#include <iostream>
#include <vector>


class Group : public Object3D {

public:

    Group() {}

    explicit Group (int num_objects): object_num(num_objects) {}

    ~Group() override {
        for (auto obj: objects) {
            delete obj;
        }
    }

    bool intersect(const Ray &r, Hit &h, float tmin) override {
        // intersect every object in objects
        // find the closest and return
        bool hasIntersect = false;
        Hit h_tmp;
        for (auto obj: objects) {
            if (obj->intersect(r, h_tmp, tmin) && h_tmp.getT() < h.getT()) {
                h = h_tmp;
                hasIntersect = true;
            }
        }
        return hasIntersect;
    }

    void addObject(int index, Object3D *obj) {
        objects.push_back(obj);
    }

    int getGroupSize() {
        return object_num;
    }

private:
    std::vector<Object3D*> objects;
    int object_num;
};

#endif
	
