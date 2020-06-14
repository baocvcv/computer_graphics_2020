#ifndef VEC_HPP_
#define VEC_HPP_

#include "common.hpp"

struct Vec3
{
    double x, y, z;

    Vec3(double x_=0, double y_=0, double z_=0): x(x_), y(y_), z(z_) {}
    Vec3(const Vec3& v): x(v.x), y(v.y), z(v.z) {}

    Vec3 operator-() const { return Vec3(-x, -y, -z); }
    Vec3 operator+(const Vec3& v) const { return Vec3(x+v.x, y+v.y, z+v.z); }
    Vec3 operator-(const Vec3& v) const { return Vec3(x-v.x, y-v.y, z-v.z); }
    Vec3 operator*(const Vec3& v) const { return Vec3(x*v.x, y*v.y, z*v.z); }
    Vec3 operator/(const Vec3& v) const { return Vec3(x/v.x, y/v.y, z/v.z); }
    Vec3& operator+=(const Vec3& v) { return *this = *this + v; }
    Vec3& operator-=(const Vec3& v) { return *this = *this - v; }
    Vec3& operator*=(const Vec3& v) { return *this = *this * v; }
    Vec3& operator/=(const Vec3& v) { return *this = *this / v; }
    
    Vec3 operator+(double a) const { return Vec3(x+a, y+a, z+a); }
    Vec3 operator-(double a) const { return Vec3(x-a, y-a, z-a); }
    Vec3 operator*(double a) const { return Vec3(x*a, y*a, z*a); }
    Vec3 operator/(double a) const { return Vec3(x/a, y/a, z/a); }
    Vec3& operator+=(double a) { return *this = *this + a; }
    Vec3& operator-=(double a) { return *this = *this - a; }
    Vec3& operator*=(double a) { return *this = *this * a; }
    Vec3& operator/=(double a) { return *this = *this / a; }

    bool operator==(const Vec3& v) const { return x==v.x && y==v.y && z==v.z; }
    bool operator!=(const Vec3& v) const { return !(*this == v); }

    double dot(const Vec3& v) const { return x*v.x + y*v.y + z*v.z; }
    double operator^(const Vec3& v) const { return x*v.x + y*v.y + z*v.z; }
    Vec3 cross(const Vec3& v) const {
        return Vec3(y*v.z-z*v.y, z*v.x-x*v.z, x*v.y-y*v.x);
    }
    Vec3 operator%(const Vec3& v) const {
        return Vec3(y*v.z-z*v.y, z*v.x-x*v.z, x*v.y-y*v.x);
    }
    void normalize() { *this = *this / len(); }
    Vec3 normalized() const { return *this / len(); }
    Vec3 clamped() const { return Vec3(clamp(x), clamp(y), clamp(z)); } 
    
    Vec3 reflect(const Vec3& n) const { return *this - n * dot(n) * 2.; }
    // refract ??

    double len2() const { return x*x + y*y + z*z; }
    double len() const { return sqrt(len2()); }

    double max() const { return x>y && x>z ? x : y>z ? y : z; }
    bool non_zero() const {
        return x>eps || x<-eps || y>eps || y<-eps || z>eps || z<-eps;
    }
};

Vec3 min(const Vec3& a, const Vec3& b) {
    return Vec3(std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z));
}

Vec3 max(const Vec3& a, const Vec3& b) {
    return Vec3(std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z));
}

inline std::ostream &operator<<(std::ostream &os, const Vec3& v) {
    os << "Vec(" << v.x << ", " << v.y << ", " << v.z << ") ";
    return os;
}
#endif