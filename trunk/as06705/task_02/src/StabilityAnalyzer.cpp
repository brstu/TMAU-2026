#include "StabilityAnalyzer.hpp"
#include <algorithm>
#include <cmath>
#include <sstream>

namespace stability {

    std::vector<std::complex<double>> findRoots(const std::vector<double>& c,
        int maxIter, double tol) {
        if (c.empty()) return {};

        std::vector<double> coeffs = c;

        size_t start = 0;
        while (start < coeffs.size() && std::abs(coeffs[start]) < 1e-15) ++start;
        if (start >= coeffs.size()) return {};

        int zerosAtOrigin = 0;
        while (coeffs.size() - start > 1 && std::abs(coeffs.back()) < 1e-15) {
            coeffs.pop_back();
            ++zerosAtOrigin;
        }

        const int n = static_cast<int>(coeffs.size() - start) - 1;
        std::vector<std::complex<double>> roots;

        if (n > 0) {
            const double lead = coeffs[start];
            std::vector<double> p(n + 1);
            for (int i = 0; i <= n; ++i) p[i] = coeffs[start + i] / lead;

            std::vector<std::complex<double>> dk(n);
            std::complex<double> seed(0.4, 0.9), acc(1.0, 0.0);
            for (int i = 0; i < n; ++i) { dk[i] = acc; acc *= seed; }

            auto eval = [&](std::complex<double> z) {
                std::complex<double> r = p[0];
                for (int i = 1; i <= n; ++i) r = r * z + p[i];
                return r;
                };

            for (int it = 0; it < maxIter; ++it) {
                double maxDelta = 0.0;
                for (int i = 0; i < n; ++i) {
                    std::complex<double> num = eval(dk[i]);
                    std::complex<double> den(1.0, 0.0);
                    for (int j = 0; j < n; ++j)
                        if (i != j) den *= (dk[i] - dk[j]);
                    if (std::abs(den) < 1e-30) continue;
                    std::complex<double> delta = num / den;
                    dk[i] -= delta;
                    maxDelta = std::max(maxDelta, std::abs(delta));
                }
                if (maxDelta < tol) break;
            }
            roots = dk;
        }

        for (int i = 0; i < zerosAtOrigin; ++i)
            roots.emplace_back(0.0, 0.0);
        return roots;
    }

    double spectralRadius(const std::vector<double>& c) {
        auto roots = findRoots(c);
        double r = 0.0;
        for (const auto& z : roots) r = std::max(r, std::abs(z));
        return r;
    }

    AnalysisResult analyzeModel16(double a1, double a2, double a3) {
        AnalysisResult res;
        const std::vector<double> coeffs = { 1.0, -a1, -a2, -a3 };
        res.roots = findRoots(coeffs);
        for (const auto& z : res.roots)
            res.radius = std::max(res.radius, std::abs(z));

        const double eps = 1e-6;
        res.stable = res.radius < 1.0 - eps;

        std::ostringstream os;
        os << "Model 1.6 spectral radius |z|max = " << res.radius;
        if (res.stable)                                          os << " -> STABLE";
        else if (std::abs(res.radius - 1.0) <= eps)              os << " -> MARGINAL (unit circle)";
        else                                                     os << " -> UNSTABLE (divergent)";
        res.message = os.str();
        return res;
    }

} 