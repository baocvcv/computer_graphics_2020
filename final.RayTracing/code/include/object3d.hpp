#ifndef OBJECT3D_H
#define OBJECT3D_H

#include "helpers.hpp"

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
    Vector3f p;
    Vector3f normal;
    float d;

    Plane(): Object3D() {}

    Plane(const Vector3f &normal_, const Vector3f &p_, Material *m) :
        Object3D(m), p(p_), normal(normal_) {
            d = Vector3f::dot(normal_, p_);
        }

    ~Plane() override = default;

    bool intersect(const Ray &r, Hit &h, float tmin) override {
        // ray r is parallel with the plane
        if (abs_f(Vector3f::dot(r.dir, normal)) < 1e-8)
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
        float s = d - Vector3f::dot(normal, r.origin);
        float t = s / Vector3f::dot(normal, r.dir);
        return t;
    }
};

class Sphere : public Object3D {
public:
    Vector3f center;
    double radius;

    // unit ball at the center
    Sphere() : center(Vector3f::ZERO), radius(1.) {}

    Sphere(const Vector3f &_center, float _radius, Material *_material) :
        Object3D(_material), center(_center), radius(_radius) {}

    ~Sphere() override = default;

    bool intersect(const Ray &r, Hit &h, float tmin) override {
        // Calc dist from center to ray r
        Vector3f originToCent = center - r.origin;
        double d1 = abs_f(Vector3f::dot(r.dir.normalized(), originToCent));
        double d2 = originToCent.squaredLength() - d1 * d1; // center to ray r
        // Cals intersection
        double interHalfLen = sqrt(radius*radius - d2);
        double t = d1 - interHalfLen;
        if (t < tmin) return false;
        else {
            Vector3f normal = r.pointAtParameter(t) - center;
            h.set(t, material, normal.normalized());
            return true;
        }
    }
};

// transforms a 3D point using a matrix, returning a 3D point
static Vector3f transformPoint(const Matrix4f &mat, const Vector3f &point) {
    return (mat * Vector4f(point, 1)).xyz();
}

// transform a 3D directino using a matrix, returning a direction
static Vector3f transformDirection(const Matrix4f &mat, const Vector3f &dir) {
    return (mat * Vector4f(dir, 0)).xyz();
}

// TODO: implement this class so that the intersect function first transforms the ray
class Transform : public Object3D {
public:
    Object3D *o; //un-transformed object
    Matrix4f transform;

    Transform() {}

    Transform(const Matrix4f &m, Object3D *obj) : o(obj) { transform = m.inverse(); }

    ~Transform() {}

    virtual bool intersect(const Ray &r, Hit &h, float tmin) {
        Vector3f trSource = transformPoint(transform, r.origin);
        Vector3f trDirection = transformDirection(transform, r.dir);
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
	Vector3f vertices[3];

	Triangle() = delete;

    // a b c are three vertex positions of the triangle
	Triangle( const Vector3f& a, const Vector3f& b, const Vector3f& c, Material* m) :
        Plane(Vector3f::cross(b-a, c-a).normalized(), a, m), vertices{a, b, c} {}

	bool intersect( const Ray& ray,  Hit& hit , float tmin) override {
		// solve the intersection between ray and the triangle plane
		float t = solve(ray);
		Vector3f p = ray.pointAtParameter(t); // the intersection point

		// determin if is inside the triangle
		// using the Barycentric Technique
		// see https://blackpawn.com/texts/pointinpoly/default.html
		Vector3f v0 = vertices[2] - vertices[0];
		Vector3f v1 = vertices[1] - vertices[0];
		Vector3f v2 = p - vertices[0];
		auto dot = Vector3f::dot;
		float denominator = v0.squaredLength()*v1.squaredLength() - dot(v0, v1)*dot(v1, v0);
		float u = (v1.squaredLength()*dot(v2, v0) - dot(v1, v0)*dot(v2, v1)) / denominator;
		float v = (v0.squaredLength()*dot(v2, v1) - dot(v0, v1)*dot(v2, v0)) / denominator;
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
    Vector3f u, v;
    float u_len_inv, v_len_inv;

    Rectangle() {}

    Rectangle(Vector3f p_, Vector3f u_, Vector3f v_, Material *m) :
        Plane(Vector3f::cross(u_, v_), p_, m), u(u_.normalized()),
        v(v_.normalized()), u_len_inv(1./u_.length()),
        v_len_inv(1./v_.length()) {}
    
    bool intersect(const Ray& r, Hit& h, float tmin) override {
        float t = solve(r);
        Vector3f p_to_x = r.pointAtParameter(t) - p; // p -> point of intersection
        double du = Vector3f::dot(p_to_x, u);
        double dv = Vector3f::dot(p_to_x, v);
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
    Vector3f u, v;

    Circle(Vector3f c, float r_, Vector3f n, Material *m): Plane(n, c, m), r(r_) {
        if (n.x() < 1.0) {
            u.x() = n.z();
            u.y() = 0.;
            u.z() = -n.x();
        } else {
            u.x() = 0.;
            u.y() = -n.z();
            u.z() = n.y();
        }
        u = u.normalized() * r;
        v = Vector3f::cross(n, u).normalized() * r;
    }

    bool intersect(const Ray& ray, Hit& h, float tmin) override {
        float t = solve(ray);
        Vector3f x = ray.pointAtParameter(t);
        double dist = (x - p).length();
        if (t > tmin && dist < r) {
            h.set(t, material, normal);
            return true;
        }
        return false;
    }
};
#endif

