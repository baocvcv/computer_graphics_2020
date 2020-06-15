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

    Mesh(const char *filename, Material *m) {
        // Optional: Use tiny obj loader to replace this simple one.
        std::ifstream f;
        f.open(filename);
        if (!f.is_open()) {
            std::cout << "Cannot open " << filename << "\n";
            return;
        }
        std::string line;
        std::string vTok("v");
        std::string fTok("f");
        std::string texTok("vt");
        char bslash = '/', space = ' ';
        std::string tok;
        int texID;
        while (true) {
            std::getline(f, line);
            if (f.eof()) { break; }
            if (line.size() < 3) { continue; }
            if (line.at(0) == '#') { continue; }
            std::stringstream ss(line);
            ss >> tok;
            if (tok == vTok) {
                Vec3 vec;
                ss >> vec.x >> vec.y >> vec.z;
                v.push_back(vec);
            } else if (tok == fTok) {
                if (line.find(bslash) != std::string::npos) {
                    std::replace(line.begin(), line.end(), bslash, space);
                    std::stringstream facess(line);
                    TriangleIndex trig;
                    facess >> tok;
                    for (int ii = 0; ii < 3; ii++) {
                        facess >> trig[ii] >> texID;
                        trig[ii]--;
                    }
                    t.push_back(trig);
                } else {
                    TriangleIndex trig;
                    for (int ii = 0; ii < 3; ii++) {
                        ss >> trig[ii];
                        trig[ii]--;
                    }
                    t.push_back(trig);
                }
            } else if (tok == texTok) {
                Vec3 texcoord;
                ss >> texcoord.x;
                ss >> texcoord.y;
            }
        }
        f.close();
        computeNormal();
    }

    bool intersect(const Ray &r, Hit &h, float tmin) override {
        // Optional: Change this brute force method into a faster one.
        // TODO: kdtree here
        bool result = false;
        Hit h_tmp;
        for (int triId = 0; triId < (int) t.size(); ++triId) {
            TriangleIndex& triIndex = t[triId];
            Triangle triangle(v[triIndex[0]],
                            v[triIndex[1]], v[triIndex[2]], material);
            triangle.normal = n[triId];
            result |= triangle.intersect(r, h_tmp, tmin);
            if (h_tmp.t < h.t) h = h_tmp;
        }
        return result;
    }

    // Normal can be used for light estimation
    void computeNormal() {
        n.resize(t.size());
        for (int triId = 0; triId < (int) t.size(); ++triId) {
            TriangleIndex& triIndex = t[triId];
            Vec3 a = v[triIndex[1]] - v[triIndex[0]];
            Vec3 b = v[triIndex[2]] - v[triIndex[0]];
            b = a.cross(b);
            n[triId] = b / b.len();
        }
    }
};

#endif
