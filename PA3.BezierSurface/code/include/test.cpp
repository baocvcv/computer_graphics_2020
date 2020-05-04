#include <vector>
#include <cstdio>
#include <iostream>

typedef std::pair<int, std::vector<std::pair<double, double>>> Basis;

class Bernstein {
public:
    Bernstein(int _n, int _k, const std::vector<double>& _t) :
            n(_n), k(_k), t(_t)
    {
        tpad = t;
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

int main()
{
    int n = 4;
    int k = 3;
    // std::vector<double> t(n+k+1, .0);
    // t[n+k] = 1.0;
    auto t = Bernstein::bezier_knot(k);

    printf("Knots: ");
    // for (int i = 1; i < n+k; i++) {
    //     t[i] = 1.0 * i / (n+k);
    //     printf("%.4f ", t[i]);
    // }
    for (auto ti: t) { printf("%.4f ", ti); }

    printf("\n");
    Bernstein b(n, k, t);
    for(double ti = .0; ti <= 1; ti += 1./99) {
        auto basis = b.evaluate(ti);
        printf("%.4f: [", ti);
        for (auto v: basis.second) {
            printf("%.5f ", v.first);
        }
        printf("] [");
        for (auto v: basis.second) {
            printf("%.5f ", v.second);
        }
        printf("] %d\n", basis.first);
    }
    return 0;
}