#ifndef REVSURFACE_HPP
#define REVSURFACE_HPP

#include "object3d.hpp"
#include "curve.hpp"
#include <tuple>

class RevSurface : public Object3D {

    Curve *pCurve;

public:
    RevSurface(Curve *pCurve, Material* material) : pCurve(pCurve), Object3D(material) {
        // Check flat.
        for (const auto &cp : pCurve->getControls()) {
            if (cp.z != 0.0) {
                printf("Profile of revSurface must be flat on xy plane.\n");
                exit(0);
            }
        }
    }

    ~RevSurface() override {
        delete pCurve;
    }

    bool intersect(const Ray &r, Hit &h, float tmin) override {
        // TODO: implement this for the ray-tracing routine using G-N iteration.
        return false;
    }

    // void drawGL() override {
    //     Object3D::drawGL();

    //     // Definition for drawable surface.
    //     typedef std::tuple<unsigned, unsigned, unsigned> Tup3u;
    //     // Surface is just a struct that contains vertices, normals, and
    //     // faces.  VV[i] is the position of vertex i, and VN[i] is the normal
    //     // of vertex i.  A face is a triple i,j,k corresponding to a triangle
    //     // with (vertex i, normal i), (vertex j, normal j), ...
    //     // Currently this struct is computed every time when canvas refreshes.
    //     // You can store this as member function to accelerate rendering.

    //     struct Surface {
    //         std::vector<Vec3> VV;
    //         std::vector<Vec3> VN;
    //         std::vector<Tup3u> VF;
    //     } surface;

    //     std::vector<CurvePoint> curvePoints;
    //     pCurve->discretize(30, curvePoints);
    //     const int steps = 40;
    //     for (unsigned int ci = 0; ci < curvePoints.size(); ++ci) {
    //         const CurvePoint &cp = curvePoints[ci];
    //         for (unsigned int i = 0; i < steps; ++i) {
    //             float t = (float) i / steps;
    //             Quat4f rot;
    //             rot.setAxisAngle(t * 2 * 3.14159, Vec3::UP);
    //             Vec3 pnew = Matrix3f::rotation(rot) * cp.V;
    //             Vec3 pNormal = Vec3::cross(cp.T, -Vec3::FORWARD);
    //             Vec3 nnew = Matrix3f::rotation(rot) * pNormal;
    //             surface.VV.push_back(pnew);
    //             surface.VN.push_back(nnew);
    //             int i1 = (i + 1 == steps) ? 0 : i + 1;
    //             if (ci != curvePoints.size() - 1) {
    //                 surface.VF.emplace_back((ci + 1) * steps + i, ci * steps + i1, ci * steps + i);
    //                 surface.VF.emplace_back((ci + 1) * steps + i, (ci + 1) * steps + i1, ci * steps + i1);
    //             }
    //         }
    //     }

    //     glBegin(GL_TRIANGLES);
    //     for (unsigned i = 0; i < surface.VF.size(); i++) {
    //         glNormal3fv(surface.VN[std::get<0>(surface.VF[i])]);
    //         glVertex3fv(surface.VV[std::get<0>(surface.VF[i])]);
    //         glNormal3fv(surface.VN[std::get<1>(surface.VF[i])]);
    //         glVertex3fv(surface.VV[std::get<1>(surface.VF[i])]);
    //         glNormal3fv(surface.VN[std::get<2>(surface.VF[i])]);
    //         glVertex3fv(surface.VV[std::get<2>(surface.VF[i])]);
    //     }
    //     glEnd();
    // }
};

#endif //REVSURFACE_HPP
