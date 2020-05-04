#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "object3d.hpp"
#include "plane.hpp"
#include <vecmath.h>
#include <cmath>
#include <iostream>
using namespace std;

class Triangle: public Object3D {

public:
	Triangle() = delete;

    // a b c are three vertex positions of the triangle
	Triangle( const Vector3f& a, const Vector3f& b, const Vector3f& c, Material* m) :
		Object3D(m), vertices({a, b, c}) {
			Vector3f ab = b - a;
			Vector3f ac = c - a;
			normal = Vector3f::cross(ab, ac).normalized();
		}

	bool intersect( const Ray& ray,  Hit& hit , float tmin) override {
		// solve the intersection between ray and the triangle plane
		float d = Vector3f::dot(normal, vertices[0]);
		float t = Plane::solve(ray, normal, d);
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

	Vector3f normal;
	Vector3f vertices[3];
protected:

};

#endif //TRIANGLE_H
