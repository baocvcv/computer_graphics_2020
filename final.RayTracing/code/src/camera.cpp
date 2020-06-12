#include "camera.hpp"
#include <vector>

using namespace std;

// void PerspectiveCamera::renderFrame(const SceneParser& sp, Image& outImg, int n_samples) {
//     outImg.SetSize(width, height);
//     Group *group = sp.getGroup();
//     int numlights = sp.getNumLights();
//     vector<Light*> lights;
//     for (int i = 0; i < numlights; i++) lights.push_back(sp.getLight(i));

//     Vector3f bkgColor = sp.getBackgroundColor();
//     double tmin = 1e-6;
//     for (int x = 0; x < width; x++) {
//         for (int y = 0; y < height; y++) {
//             Hit h;
//             Ray r = generateRay(Vector2f(x, y));
//             if (group->intersect(r, h, tmin)) {
//                 Vector3f color = Vector3f::ZERO;
//                 for (auto light: lights) {
//                     Vector3f lightColor, dirToLight;
//                     Vector3f p = r.pointAtParameter(h.t);
//                     light->getIllumination(p, dirToLight, lightColor);
//                     color += h.material->Shade(r, h, dirToLight, lightColor);
//                 }
//                 outImg.SetPixel(x, y, color);
//             } else { outImg.SetPixel(x, y, bkgColor); }
//         }
//     }
// }

Vector3f radiance(const Ray &r, int depth, Group *group, unsigned short *Xi) {
    if (depth >= 5) return Vector3f::ZERO;

    Hit h;
    double tmin = 1e-6;
    Vector3f color = Vector3f::ZERO;
    if (group->intersect(r, h, tmin)) {
        //TODO : smallpt

        Vector3f x = r.pointAtParameter(h.t);
        Vector3f n = h.normal;
        color = h.material->emission;
        switch (h.material->type)
        {
            case Material::MaterialType::DIFFUSE: {

                auto diffuse = h.material->diffuseRay(r, h, Xi);
                color += h.material->color * radiance(diffuse, depth+1, group, Xi);
                break;
            }
            case Material::MaterialType::SPECULAR: {
                auto spec = h.material->specularRay(r, h);
                color += h.material->color * radiance(spec, depth+1, group, Xi);
                break;
            }
            case Material::MaterialType::REFRACT: {
                auto rays = h.material->refractiveRay(r, h, Xi);
                auto reflect = rays.first;
                auto refract = rays.second;
                if (depth >= 2) {
                    if (erand48(Xi) < reflect.second) {
                        color += h.material->color * radiance(reflect.first, depth+1, group, Xi);
                    } else {
                        color += h.material->color * radiance(refract.first, depth+1, group, Xi);
                    }
                } else {
                    if (refract.second < tmin) {
                        color += h.material->color * radiance(reflect.first, depth+1, group, Xi);
                    } else {
                        color += h.material->color * (radiance(reflect.first, depth+1, group, Xi) * reflect.second
                            + radiance(refract.first, depth+1, group, Xi) * refract.second);
                    }
                }
            }
        }
    }
    return color;
}

inline double clamp(double x) { return x < 0 ? 0 : x > 1 ? 1 : x; }
inline int toInt(double x) { return int(pow(clamp(x), 1 / 2.2) * 255 + .5); }

void PerspectiveCamera::renderFrame(const SceneParser& sp, Image& outImg, int samps) {
    outImg.SetSize(width, height);
    int w = width, h = height;

    Group *group = sp.getGroup();
    
    Vector3f r;
    #pragma omp parallel for schedule(dynamic, 1) private(r)
    for (int y = 0; y < h; y++)
    { // Loop over image rows
        fprintf(stderr, "\rRendering (%d spp) %5.2f%%", samps * 4, 100. * y / (h - 1));
        for (unsigned short x = 0, Xi[3] = {0, 0, y * y * y}; x < w; x++) {// Loop cols
            outImg.SetPixel(x, y, Vector3f::ZERO);
            for (int sy = 0; sy < 2; sy++) {      // 2x2 subpixel rows, i = index of pixels unrolled
                for (int sx = 0; sx < 2; sx++) { // 2x2 subpixel cols
                    r = Vector3f::ZERO;
                    for (int s = 0; s < samps; s++) {
                        double r1 = 2 * erand48(Xi), r2 = 2 * erand48(Xi);
                        double dx = r1 < 1 ? sqrt(r1) - 1 : 1 - sqrt(2 - r1);
                        double dy = r2 < 1 ? sqrt(r2) - 1 : 1 - sqrt(2 - r2);

                        Vector2f p((sx + .5 + dx) / 2 + x, (sy + .5 + dy) / 2 + y);
                        Ray d = generateRay(p);
                        // Vec d = cx * (((sx + .5 + dx) / 2 + x) / w - .5) +
                        //         cy * (((sy + .5 + dy) / 2 + y) / h - .5) + cam.d;
                        r += radiance(d, 0, group, Xi) * (1. / samps);
                    }
                    outImg.IncrementPixel(x, y, Vector3f(clamp(r.x()), clamp(r.y()), clamp(r.z())) * 0.25);
                }
            }
        }
    }
}