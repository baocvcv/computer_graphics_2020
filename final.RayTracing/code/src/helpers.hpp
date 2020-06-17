#ifndef HELPERS_H_
#define HELPERS_H_

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif

#include "common.hpp"
#include "vec.hpp"

class Material;

// Ray class mostly copied from Peter Shirley and Keith Morley
class Ray {
public:
    Vec3 origin;
    Vec3 dir;

    Ray() = delete;
    Ray(const Vec3 &orig, const Vec3 &_dir) : origin(orig), dir(_dir) {}
    Ray(const Ray &r) { origin = r.origin, dir = r.dir; }
    Vec3 pointAtParameter(double t) const { return origin + dir * t; }
};

inline std::ostream &operator<<(std::ostream &os, const Ray &r) {
    os << "Ray <" << r.origin << ", " << r.dir << ">";
    return os;
}

// TODO: add u,v for texture mapping here
class Hit {
public:
    double t;
    Material *material;
    Vec3 normal;
    Vec3 uv;

    Hit() : material(nullptr), t(1e38) {}

    Hit(double _t, Material *m, const Vec3 &n, const Vec3& uv_=Vec3()) :
        t(_t), material(m), normal(n), uv(uv_) {}

    Hit(const Hit &h) {
        t = h.t;
        material = h.material;
        normal = h.normal;
        uv = h.uv;
    }

    void set(double _t, Material *_m, const Vec3 &n, const Vec3& uv_=Vec3()) {
        t = _t;
        material = _m;
        normal = n;
        uv = uv_;
    }
};

inline std::ostream &operator<<(std::ostream &os, const Hit &h) {
    os << "Hit <" << h.t << ", " << h.normal << ">";
    return os;
}

class Material {
public:
    MaterialType type;
    Vec3 color;
    Vec3 emission;
    double n_material; // refraction index

    unsigned char* texture_buf;
    int w, h, _c;

    Material(MaterialType t, const Vec3 &c, const Vec3 &e=Vec3(),
        double n=1., const std::string& texture="") :
            type(t), color(c), emission(e), n_material(n), texture_buf(nullptr) {
                if (texture != "") {
                    texture_buf = stbi_load(texture.c_str(), &w, &h, &_c, 0);
                }
            }

    Material(const Material& m): type(m.type), color(m.color), emission(m.emission),
        n_material(m.n_material), w(m.w), h(m.h), _c(m._c), texture_buf(nullptr) {
            if (m.texture_buf != nullptr) {
                texture_buf = new unsigned char[sizeof(m.texture_buf) / sizeof(unsigned char)];
                memcpy(texture_buf, m.texture_buf, sizeof(m.texture_buf));
            }
        }

    virtual ~Material() = default;

    Vec3 Shade(const Ray &ray, const Hit &hit,
                   const Vec3 &dirToLight, const Vec3 &lightColor) { // BRDF
        Vec3 shaded;

        // double t1 = Vector3f::dot(dirToLight, hit.normal); // Lx . N
        // Vector3f Rx = hit.normal * 2 * t1 - dirToLight;
        // t1 = ReLU(t1);
        // double t2 = ReLU(-Vector3f::dot(ray.getDirection(), Rx));
        // t2 = pow(t2, shininess);
        // shaded = lightColor * (diffuseColor * t1 + specularColor * t2);

        return shaded;
    }

    inline double ReLU(double x) { return (x > 0) ? x : .0; }

    Vec3 getColor(Vec3 uv) {
        if (texture_buf == nullptr) {
            return color;
        }
        // int pw = (((int)(uv.x*w)) % w + w) % w;
        // int ph = (((int)(uv.y*h)) % h + h) % h;
        // int pw = (int(uv.x*w)) % w;
        // int ph = (int(uv.y*h)) % h;
        int pw = uv.x * w;
        int ph = uv.y * h;
        int x = ph * w * _c + pw * _c;

        auto c = Vec3(texture_buf[x], texture_buf[x+1], texture_buf[x+2]) / 255.;
        return c;
        // return Vec3(texture_buf[x], texture_buf[x+1], texture_buf[x+2]) / 255.;
    }
};

Ray diffuseRay(const Ray &ray, const Hit &hit, unsigned short *Xi) {
    // Ideal DIFFUSE reflection
    double r1 = 2 * M_PI * erand48(Xi); // angle
    double r2 = erand48(Xi), r2s = sqrt(r2);
    Vec3 w = hit.normal;
    if (w.dot(ray.dir) > 0)
        w = -w;
    // u, v are vectors on the tangent plane at x

    Vec3 u = (fabs(w.x) > .1 ? Vec3(0, 1)
                             : Vec3(1)).cross(w).normalized();
    Vec3 v = w % u;
    // TODO: what is this?
    Vec3 d = (u * cos(r1) * r2s + v * sin(r1) * r2s + w * sqrt(1 - r2)).normalized();
    Vec3 x = ray.pointAtParameter(hit.t);
    return Ray(x, d);
}

Ray specularRay(const Ray &ray, const Hit &hit) {
    return Ray(ray.pointAtParameter(hit.t), ray.dir.reflect(hit.normal));
}

// TODO: currently assume every object is surrounded by air, can probably improve to object surrounded by object?
// return <reflect, refract>
std::pair<std::pair<Ray, double>, std::pair<Ray, double>> refractiveRay(
        const Ray &ray, const Hit &hit, unsigned short *Xi) {

    double n_air = 1;
    double n_material = hit.material->n_material;
    //TODO: understande the math here
    double r0 = square(n_air - n_material) / square(n_air + n_material);
    auto p = ray.pointAtParameter(hit.t);
    Ray reflect = Ray(p, ray.dir.reflect(hit.normal));

    double cos_theta = ray.dir.dot(hit.normal);
    double sin_theta = sqrt(1 - cos_theta * cos_theta);
    double n;
    Vec3 norm = hit.normal;
    if (cos_theta < 0) { // from inside
        n = n_material / n_air;
        cos_theta = -cos_theta;
        norm = -norm;
    } else { // from outside
        n = n_air / n_material;
        if (sin_theta >= n) {
            return { {reflect, 1.}, {reflect, .0} };
        }
    }

    Vec3 refract_d = norm * (sqrt(1 - sin_theta*sin_theta / (n*n)) - cos_theta/n) + ray.dir / n;
    Ray refract(p, refract_d);
    double reflect_i = r0 + (1. - r0) * pow((1. - cos_theta), 5);
    double refract_i = 1. - reflect_i;
    return { {reflect, reflect_i}, {refract, refract_i} };
}

#endif