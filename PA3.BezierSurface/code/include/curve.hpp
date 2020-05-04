#ifndef CURVE_HPP
#define CURVE_HPP

#include "object3d.hpp"
#include <vecmath.h>
#include <vector>
#include <utility>
#include <cstdio>

#include <algorithm>

// TODO (PA3): Implement Bernstein class to compute spline basis function.
//       You may refer to the python-script for implementation.
typedef std::pair<int, std::vector<std::pair<double, double>>> Basis;

class Bernstein {
public:
    Bernstein(int _n, int _k, const std::vector<double>& _t) :
            n(_n), k(_k), t(_t), tpad(_t)
    {
        std::vector<double> pad(k, t.back());
        tpad.insert(tpad.end(), pad.begin(), pad.end());
    }

    static std::vector<double> bezier_knot(int k)
    {
        std::vector<double> v0(k + 1, .0), v1(k+1, 1.);
        v0.insert(v0.end(), v1.begin(), v1.end());
        return v0;
    }

    int get_bpos(double mu)
    {
        if (mu < t[0] || t.back() < mu) {
            return -1;
        }
        if (mu == t[0]) {
            for (auto bpos = t.size() - 1; bpos >= 0; bpos--) {
                if (t[bpos] <= mu) {
                    return bpos;
                }
            }
        } else {
            for (auto bpos = t.size() - 1; bpos >= 0; bpos--) {
                if (t[bpos] < mu) {
                    return bpos;
                }
            }
        }
    }

    std::pair<double, double> get_valid_range()
    {
        return std::make_pair(t[k], t[t.size()-k-1]);
    }

    Basis evaluate(double mu)
    {
        auto bpos = get_bpos(mu);
        std::vector<double> s(k+2, .0);
        s[k] = 1;
        std::vector<double> ds(k+1, 1.);
        for (int p = 1; p < k + 1; p++) {
            for (int ii = k - p; ii < k + 1; ii++) {
                int i = ii + bpos - k;
                double w1, w2, dw1, dw2;
                if (tpad[i+p] == tpad[i]) {
                    w1 = mu;
                    dw1 = 1;
                } else {
                    w1 = (mu - tpad[i]) / (tpad[i+p] - tpad[i]);
                    dw1 = 1 / (tpad[i+p] - tpad[i]);
                }
                if (tpad[i+p+1] == tpad[i+1]) {
                    w2 = 1 - mu;
                    dw2 = -1;
                } else {
                    w2 = (tpad[i+p+1] - mu) / (tpad[i+p+1] - tpad[i+1]);
                    dw2 = -1 / (tpad[i+p+1] - tpad[i+1]);
                }
                if (p == k) {
                    ds[ii] = (dw1 * s[ii] + dw2 * s[ii+1]) * p;
                }
                s[ii] = w1 * s[ii] + w2 * s[ii+1];
            }
        }
        s.pop_back();
        int lsk = bpos - k;
        int rsk = n - bpos - 1;
        if (lsk < 0) {
            std::vector<decltype(s)::value_type>(s.begin()+(-lsk), s.end()).swap(s);
            std::vector<decltype(ds)::value_type>(ds.begin()+(-lsk), ds.end()).swap(ds);
            lsk = 0;
        }
        if (rsk < 0) {
            while (rsk++ < 0) {
                s.pop_back();
                ds.pop_back();
            }
        }
        std::vector<std::pair<double, double>> sds;
        for (int i = 0; i < s.size(); i++) {
            sds.emplace_back(s[i], ds[i]);
        }
        return { lsk, sds };
    }

private:
    int n, k;
    std::vector<double> t;
    std::vector<double> tpad;
};

// The CurvePoint object stores information about a point on a curve
// after it has been tesselated: the vertex (V) and the tangent (T)
// It is the responsiblility of functions that create these objects to fill in all the data.
struct CurvePoint {
    Vector3f V; // Vertex
    Vector3f T; // Tangent  (unit)

    CurvePoint(const Vector3f& _V, const Vector3f& _T) :
        V(_V), T(_T) {}
};

class Curve : public Object3D {
protected:
    std::vector<Vector3f> controls;
public:
    explicit Curve(std::vector<Vector3f> points) : controls(std::move(points)) {}

    bool intersect(const Ray &r, Hit &h, float tmin) override {
        return false;
    }

    std::vector<Vector3f> &getControls() {
        return controls;
    }

    virtual void discretize(int resolution, std::vector<CurvePoint>& data) = 0;

    void drawGL() override {
        Object3D::drawGL();
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        glDisable(GL_LIGHTING);
        glColor3f(1, 1, 0);
        glBegin(GL_LINE_STRIP);
        for (auto & control : controls) { glVertex3fv(control); }
        glEnd();
        glPointSize(4);
        glBegin(GL_POINTS);
        for (auto & control : controls) { glVertex3fv(control); }
        glEnd();
        std::vector<CurvePoint> sampledPoints;
        discretize(30, sampledPoints);
        glColor3f(1, 1, 1);
        glBegin(GL_LINE_STRIP);
        for (auto & cp : sampledPoints) { glVertex3fv(cp.V); }
        glEnd();
        glPopAttrib();
    }
};

class BezierCurve : public Curve {
public:
    explicit BezierCurve(const std::vector<Vector3f> &points) : Curve(points) {
        if (points.size() < 4 || points.size() % 3 != 1) {
            printf("Number of control points of BezierCurve must be 3n+1!\n");
            exit(0);
        }
    }

    void discretize(int resolution, std::vector<CurvePoint>& data) override {
        data.clear();
        // TODO (PA3): fill in data vector
        int n = controls.size();
        auto knots = Bernstein::bezier_knot(n-1);
        Bernstein bern(n, n-1, knots);

        std::vector<double> t(resolution, .0);
        t.back() = 1.;
        for (int i = 1; i < resolution-1; i++) {
            t[i] = 1.0 * i / (resolution - 1);
        }

        for (auto ti : t) {
            auto basis = bern.evaluate(ti);
            int len = basis.second.size();
            auto vertex = Vector3f::ZERO;
            auto tangent = Vector3f::ZERO;
            for (int i = 0; i < len; i++) {
                int ii = i + basis.first;
                vertex += controls[ii] * basis.second[i].first;
                tangent += controls[ii] * basis.second[i].second;
            }
            data.emplace_back(std::move(vertex), tangent.normalized());
        }
    }

protected:

};

class BsplineCurve : public Curve {
public:
    BsplineCurve(const std::vector<Vector3f> &points) : Curve(points) {
        if (points.size() < 4) {
            printf("Number of control points of BspineCurve must be more than 4!\n");
            exit(0);
        }
    }

    void discretize(int resolution, std::vector<CurvePoint>& data) override {
        data.clear();
        // TODO (PA3): fill in data vector
        int n = controls.size();
        int k = 3;
        std::vector<double> knots(n+k+2, .0);
        for (int i = 1; i < n+k+2; i++) {
            knots[i] = i * 1.0 / (n+k+1);
        }
        Bernstein bern(n, k, knots);

        for (int i = k; i < n; i++) {
            auto step = (knots[i+1] - knots[i]) / resolution;
            for (auto ti = knots[i]; ti < knots[i+1]; ti += step) {
                auto basis = bern.evaluate(ti);
                int len = basis.second.size();
                auto vertex = Vector3f::ZERO;
                auto tangent = Vector3f::ZERO;
                for (int i = 0; i < len; i++) {
                    int ii = i + basis.first;
                    vertex += controls[ii] * basis.second[i].first;
                    tangent += controls[ii] * basis.second[i].second;
                }
                data.emplace_back(std::move(vertex), tangent.normalized());
            }
        }
    }

protected:

};

#endif // CURVE_HPP
