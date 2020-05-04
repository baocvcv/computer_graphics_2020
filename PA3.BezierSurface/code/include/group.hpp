#ifndef GROUP_H
#define GROUP_H


#include "object3d.hpp"
#include "ray.hpp"
#include "hit.hpp"
#include <iostream>
#include <vector>


class Group : public Object3D {

public:

    Group() {

    }

    explicit Group (int num_objects) {
        objects.resize(num_objects, NULL);
    }

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

    void drawGL() override {
        for (auto obj: objects) obj->drawGL();
    }

    void addObject(int index, Object3D *obj) {
        objects[index] = obj;
    }

    int getGroupSize() {
        return objects.size();
    }

private:
    std::vector<Object3D*> objects;
};

#endif
	
