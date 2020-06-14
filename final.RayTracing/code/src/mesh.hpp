#ifndef MESH_H
#define MESH_H

#include "common.hpp"
#include "object3d.hpp"
#include "vec.hpp"

class Mesh : public Object3D {
public:
    struct TriangleIndex {
        TriangleIndex() {
            x[0] = 0; x[1] = 0; x[2] = 0;
        }
        int &operator[](const int i) { return x[i]; }
        // By Computer Graphics convention, counterclockwise winding is front face
        int x[3]{};
    };

    std::vector<Vec3> v;
    std::vector<TriangleIndex> t;
    std::vector<Vec3> n;

    Mesh(const char *filename, Material *m);

    bool intersect(const Ray &r, Hit &h, float tmin) override;

    // Normal can be used for light estimation
    void computeNormal();
};

#endif
