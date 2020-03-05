#ifndef GROUP_H
#define GROUP_H


#include "object3d.hpp"
#include "ray.hpp"
#include "hit.hpp"
#include <iostream>
#include <vector>


// TODO: Implement Group - add data structure to store a list of Object*
class Group : public Object3D {

public:

    Group() {}

    explicit Group (int num_objects): object_num(num_objects) {}

    ~Group() override {
        for (auto obj: objects) {
            delete obj
        }
    }

    bool intersect(const Ray &r, Hit &h, float tmin) override {
        // TODO: do this
        // intersect every object in objects
        // find the closest and return

    }

    void addObject(int index, Object3D *obj) {
        objects.push_back(obj);
    }

    int getGroupSize() {
        return object_num;
    }

private:
    vector<Object3D*> objects;
    int object_num;
};

#endif
	
