#ifndef OBJECT3D_H
#define OBJECT3D_H

#include "helpers.hpp"
#include "common.hpp"
#include "mat44.hpp"
#include "vec.hpp"

// Base class for all 3d entities.
class Object3D {
public:
    Material *material;

    Object3D() : material(nullptr) {}

    virtual ~Object3D() = default;

    explicit Object3D(Material *material) {
        this->material = material;
    }

    // Intersect Ray with this object. If hit, store information in hit structure.
    virtual bool intersect(const Ray &r, Hit &h, float tmin) = 0;

    inline double abs_f(double x) { return (x<0 ? -x : x);}

};

// Implement Plane representing an infinite plane
// function: ax+by+cz=d
class Plane : public Object3D {
public:
    Vec3 p;
    Vec3 normal;
    float d;

    Plane(): Object3D() {}

    Plane(const Vec3 &normal_, const Vec3 &p_, Material *m) :
        Object3D(m), p(p_), normal(normal_) {
            d = normal_.dot(p_);
        }

    ~Plane() override = default;

    bool intersect(const Ray &r, Hit &h, float tmin) override {
        // ray r is parallel with the plane
        if (abs_f(r.dir ^ normal) < 1e-8)
            return false;

        // not parallel
        float t = solve(r);
        if (t > tmin) {
            h.set(t, material, normal);
            return true;
        }
        return false;
    }

    float solve(const Ray &r) {
        float s = d - normal.dot(r.origin);
        float t = s / normal.dot(r.dir);
        return t;
    }
};

class Sphere : public Object3D {
public:
    Vec3 center;
    double radius;

    // unit ball at the center
    Sphere() : center(), radius(1.) {}

    Sphere(const Vec3 &_center, float _radius, Material *_material) :
        Object3D(_material), center(_center), radius(_radius) {}

    ~Sphere() override = default;

    bool intersect(const Ray &r, Hit &h, float tmin) override {
        // Calc dist from center to ray r
        Vec3 originToCent = center - r.origin;
        double d1 = abs_f(r.dir.normalized().dot(originToCent));
        double d2 = originToCent.len2() - d1 * d1; // center to ray r
        // Cals intersection
        double interHalfLen = sqrt(radius*radius - d2);
        double t = d1 - interHalfLen;
        if (t < tmin) return false;
        else {
            Vec3 normal = r.pointAtParameter(t) - center;
            h.set(t, material, normal.normalized());
            return true;
        }
    }
};

// transforms a 3D point using a matrix, returning a 3D point
static Vec3 transformPoint(const Mat44& mat, const Vec3 &point) {
    return mat.mult(point, true);
}

// transform a 3D directino using a matrix, returning a direction
static Vec3 transformDirection(const Mat44&mat, const Vec3 &dir) {
    return mat.mult(dir);
}

// TODO: implement this class so that the intersect function first transforms the ray
class Transform : public Object3D {
public:
    Object3D *o; //un-transformed object
    Mat44 transform;

    Transform() {}

    Transform(const Mat44& m, Object3D *obj): o(obj), transform(m.inversed()) {}

    ~Transform() {}

    virtual bool intersect(const Ray &r, Hit &h, float tmin) {
        Vec3 trSource = transformPoint(transform, r.origin);
        Vec3 trDirection = transformDirection(transform, r.dir);
        Ray tr(trSource, trDirection);
        bool inter = o->intersect(tr, h, tmin);
        if (inter) {
            h.set(h.t, h.material, transformDirection(transform.transposed(), h.normal).normalized());
        }
        return inter;
    }
};


class Triangle: public Plane {
public:
	Vec3 vertices[3];

	Triangle() = delete;

    // a b c are three vertex positions of the triangle
	Triangle( const Vec3& a, const Vec3& b, const Vec3& c, Material* m) :
        Plane(b-a.cross(c-a).normalized(), a, m), vertices{a, b, c} {}

	bool intersect( const Ray& ray,  Hit& hit , float tmin) override {
		// solve the intersection between ray and the triangle plane
		float t = solve(ray);
		Vec3 p = ray.pointAtParameter(t); // the intersection point

		// determin if is inside the triangle
		// using the Barycentric Technique
		// see https://blackpawn.com/texts/pointinpoly/default.html
		Vec3 v0 = vertices[2] - vertices[0];
		Vec3 v1 = vertices[1] - vertices[0];
		Vec3 v2 = p - vertices[0];
		float denominator = v0.len2()*v1.len2() - v0.dot(v1)*v1.dot(v0);
		float u = (v1.len2()*v2.dot(v0) - v1.dot(v0)*v2.dot(v1)) / denominator;
		float v = (v0.len2()*v2.dot(v1) - v0.dot(v1)*v2.dot(v0)) / denominator;
		if (good(u) && good(v) && good(u+v) && t >= tmin) {
			hit.set(t, material, normal);
			return true;
		} else
			return false;
	}
	
	inline bool good(double x) { return (0 <= x && x <= 1); }
};

class Rectangle: public Plane {
public:
    Vec3 u, v;
    float u_len_inv, v_len_inv;

    Rectangle() {}

    Rectangle(Vec3 p_, Vec3 u_, Vec3 v_, Material *m) :
        Plane(u_.cross(v_), p_, m), u(u_.normalized()),
        v(v_.normalized()), u_len_inv(1. / u_.len()),
        v_len_inv(1. / v_.len()) {}
    
    bool intersect(const Ray& r, Hit& h, float tmin) override {
        float t = solve(r);
        Vec3 p_to_x = r.pointAtParameter(t) - p; // p -> point of intersection
        double du = p_to_x.dot(u);
        double dv = p_to_x.dot(v);
        if (tmin < du && du < 1-tmin && tmin < dv && dv < 1-tmin) {
            h.set(t, material, normal);
            return true;
        }
        return false;
    }

};

class Circle: public Plane {
public:
    float r;
    Vec3 u, v;

    Circle(Vec3 c, float r_, Vec3 n, Material *m): Plane(n, c, m), r(r_) {
        if (n.x < 1.0) {
            u.x = n.z;
            u.y = 0.;
            u.z = -n.x;
        } else {
            u.x = 0.;
            u.y = -n.z;
            u.z = n.y;
        }
        u = u.normalized() * r;
        v = n.cross(u).normalized() * r;
    }

    bool intersect(const Ray& ray, Hit& h, float tmin) override {
        float t = solve(ray);
        Vec3 x = ray.pointAtParameter(t);
        double dist = (x - p).len();
        if (t > tmin && dist < r) {
            h.set(t, material, normal);
            return true;
        }
        return false;
    }
};
#endif

