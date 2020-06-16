#ifndef CURVE_HPP
#define CURVE_HPP

#include "common.hpp"
#include "object3d.hpp"

typedef std::pair<int, std::vector<std::pair<float, float>>> Basis;

class Bernstein {
public:
    Bernstein(int _n, int _k, const std::vector<float>& _t) :
            n(_n), k(_k), t(_t), tpad(_t)
    {
        std::vector<float> pad(k, t.back());
        tpad.insert(tpad.end(), pad.begin(), pad.end());
    }

    static std::vector<float> bezier_knot(int k)
    {
        std::vector<float> v0(k + 1, .0), v1(k+1, 1.);
        v0.insert(v0.end(), v1.begin(), v1.end());
        return v0;
    }

    int get_bpos(float mu) const
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

    std::pair<float, float> get_valid_range() const
    {
        return std::make_pair(t[k], t[t.size()-k-1]);
    }

    Basis evaluate(float mu) const
    {
        auto bpos = get_bpos(mu);
        std::vector<float> s(k+2, .0);
        s[k] = 1;
        std::vector<float> ds(k+1, 1.);
        for (int p = 1; p < k + 1; p++) {
            for (int ii = k - p; ii < k + 1; ii++) {
                int i = ii + bpos - k;
                float w1, w2, dw1, dw2;
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
        std::vector<std::pair<float, float>> sds;
        for (int i = 0; i < s.size(); i++) {
            sds.emplace_back(s[i], ds[i]);
        }
        return { lsk, sds };
    }

private:
    int n, k;
    std::vector<float> t;
    std::vector<float> tpad;
};

// The CurvePoint object stores information about a point on a curve
// after it has been tesselated: the vertex (V) and the tangent (T)
// It is the responsiblility of functions that create these objects to fill in all the data.
struct CurvePoint {
    Vec3 V; // Vertex
    Vec3 T; // Tangent  (unit)
    float t;

    CurvePoint(const Vec3& _V, const Vec3& _T, float _t) :
        V(_V), T(_T), t(_t) {}
};

class Curve : public Object3D {
public:
    std::vector<Vec3> controls;
    Bernstein* bern;

    explicit Curve(std::vector<Vec3> points) : controls(std::move(points)) {}

    bool intersect(const Ray &r, Hit &h, float tmin) override {
        return false;
    }

    std::vector<Vec3> &getControls() {
        return controls;
    }

    virtual void discretize(int resolution, std::vector<CurvePoint>& data) = 0;

    // returns vertex and tangent
    std::pair<Vec3, Vec3> evaluate(const Bernstein& bern, float ti) {
        auto basis = bern.evaluate(ti);
        int len = basis.second.size();
        auto vertex = Vec3();
        auto tangent = Vec3();
        for (int i = 0; i < len; i++) {
            int ii = i + basis.first;
            vertex += controls[ii] * basis.second[i].first;
            tangent += controls[ii] * basis.second[i].second;
        }
        return {vertex, tangent};
    }

    std::pair<float, float> get_valid_range() { return bern->get_valid_range(); }

    virtual std::pair<Vec3, Vec3> evaluate(float ti) = 0;
};

class BezierCurve : public Curve {
public:

    explicit BezierCurve(const std::vector<Vec3> &points) : Curve(points) {
        int n = points.size();
        if (n < 4 || n % 3 != 1) {
            printf("Number of control points of BezierCurve must be 3n+1!\n");
            exit(0);
        }
        // auto knots = Bernstein::bezier_knot(n-1);
        bern = new Bernstein(n, n-1, Bernstein::bezier_knot(n-1));
    }

    void discretize(int resolution, std::vector<CurvePoint>& data) override {
        data.clear();
        std::vector<float> t(resolution, .0);
        t.back() = 1.;
        for (int i = 1; i < resolution-1; i++) {
            t[i] = 1.0 * i / (resolution - 1);
        }

        for (auto ti : t) {
            auto result = evaluate(ti);
            data.emplace_back(std::move(result.first), result.second.normalized(), ti);
        }
    }

    std::pair<Vec3, Vec3> evaluate(float ti) {
        return Curve::evaluate(*bern, ti);
    }
};

class BsplineCurve : public Curve {
public:
    const int k = 3;
    int n;
    std::vector<float> knots;

    BsplineCurve(const std::vector<Vec3> &points):
        Curve(points), n(points.size()) {
            if (n < 4) {
                printf("Number of control points of BspineCurve must be more than 4!\n");
                exit(0);
            }
            knots.resize(n+k+2);
            for (int i = 0; i < n+k+2; i++) {
                knots[i] = i * 1.0 / (n+k+1);
            }
            bern = new Bernstein(n, k, knots);
    }

    void discretize(int resolution, std::vector<CurvePoint>& data) override {
        data.clear();
        for (int i = k; i < n; i++) {
            auto step = (knots[i+1] - knots[i]) / resolution;
            for (auto ti = knots[i]; ti < knots[i+1]; ti += step) {
                auto result = evaluate(ti);
                data.emplace_back(std::move(result.first), result.second.normalized(), ti);
            }
        }
    }

    std::pair<Vec3, Vec3> evaluate(float ti) {
        return Curve::evaluate(*bern, ti);
    }
};

#endif // CURVE_HPP
