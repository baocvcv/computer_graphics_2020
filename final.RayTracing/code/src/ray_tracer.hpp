#include "common.hpp"
#include "camera.hpp"
#include "scenes.hpp"
#include "helpers.hpp"
#include "vec.hpp"
#include "mat44.hpp"

#include "scene_parser.hpp"

using namespace std;

Vec3 radiance(const Ray &r, int depth, Group *group, unsigned short *Xi) {
    if (depth >= 5) return Vec3();

    Hit h;
    Vec3 color;
    if (group->intersect(r, h, eps)) {
        //TODO : smallpt

        Vec3 x = r.pointAtParameter(h.t);
        Vec3 n = h.normal;
        color = h.material->emission;
        auto material_color = h.material->getColor(h.uv);
        switch (h.material->type)
        {
            case MaterialType::DIFFUSE: {

                auto diffuse = diffuseRay(r, h, Xi); //h.material->diffuseRay(r, h, Xi);
                color += material_color * radiance(diffuse, depth+1, group, Xi);
                break;
            }
            case MaterialType::SPECULAR: {
                auto spec = specularRay(r, h);
                color += material_color * radiance(spec, depth+1, group, Xi);
                break;
            }
            case MaterialType::REFRACTIVE: {
                auto rays = refractiveRay(r, h, Xi);
                auto reflect = rays.first;
                auto refract = rays.second;
                if (depth >= 2) {
                    if (erand48(Xi) < reflect.second) {
                        color += material_color * radiance(reflect.first, depth+1, group, Xi);
                    } else {
                        color += material_color * radiance(refract.first, depth+1, group, Xi);
                    }
                } else {
                    if (refract.second < eps) {
                        color += material_color * radiance(reflect.first, depth+1, group, Xi);
                    } else {
                        color += material_color * (radiance(reflect.first, depth+1, group, Xi) * reflect.second
                            + radiance(refract.first, depth+1, group, Xi) * refract.second);
                    }
                }
            }
        }
    }
    return color;
}

void renderFrame(const Scene& sp, Image& outImg, int samps) {
    auto cam = sp.camera;
    outImg.SetSize(cam->width, cam->height);
    int w = cam->width, h = cam->height;

    Group *group = sp.group;
    
    Vec3 r;
    #pragma omp parallel for schedule(dynamic, 1) private(r)
    for (int y = 0; y < h; y++)
    { // Loop over image rows
        fprintf(stderr, "\rRendering (%d spp) %5.2f%%", samps * 4, 100. * y / (h - 1));
        for (unsigned short x = 0, Xi[3] = {0, 0, (unsigned short) (y*y*y)}; x < w; x++) {
            outImg.SetPixel(x, y, Vec3());
            for (int sy = 0; sy < 2; sy++) {      // 2x2 subpixel rows, i = index of pixels unrolled
                for (int sx = 0; sx < 2; sx++) { // 2x2 subpixel cols
                    r = Vec3();
                    for (int s = 0; s < samps; s++) {
                        double r1 = 2 * erand48(Xi), r2 = 2 * erand48(Xi);
                        double dx = r1 < 1 ? sqrt(r1) - 1 : 1 - sqrt(2 - r1);
                        double dy = r2 < 1 ? sqrt(r2) - 1 : 1 - sqrt(2 - r2);

                        Vec3 p((sx + .5 + dx) / 2 + x, (sy + .5 + dy) / 2 + y);
                        Ray d = cam->generateRay(p);
                        // Vec d = cx * (((sx + .5 + dx) / 2 + x) / w - .5) +
                        //         cy * (((sy + .5 + dy) / 2 + y) / h - .5) + cam.d;
                        r += radiance(d, 0, group, Xi) * (1. / samps);
                    }
                    outImg.IncrementPixel(x, y, Vec3(clamp(r.x), clamp(r.y), clamp(r.z)) * 0.25);
                }
            }
        }
    }
}

void renderFrame(const SceneParser& sp, Image& outImage, int sampls) {
    Scene sc(sp.camera, sp.group);
    renderFrame(sc, outImage, sampls);
}