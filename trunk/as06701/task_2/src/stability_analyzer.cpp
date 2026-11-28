#include "stability_analyzer.h"
#include <algorithm>
#include <cmath>
#include <sstream>

namespace {
    constexpr double kTolerance = 1e-9;

    /** Значение полинома в точке z (схема Горнера). */
    std::complex<double> evalPoly(const std::vector<double>& coeffs,
                                  std::complex<double> z) {
        std::complex<double> result = 0.0;
        for (auto it = coeffs.rbegin(); it != coeffs.rend(); ++it) {
            result = result * z + *it;
        }
        return result;
                                  }
} // namespace

std::vector<std::complex<double>> StabilityAnalyzer::findRoots(
    const std::vector<double>& coeffs) {
    // Обрезаем ведущие нули.
    std::vector<double> c = coeffs;
    while (c.size() > 1 && std::abs(c.back()) < 1e-15) c.pop_back();
    if (c.size() <= 1) return {};

    const int n = static_cast<int>(c.size()) - 1;  // степень полинома

    // Нормируем на монический (старший коэффициент = 1).
    const double lead = c.back();
    for (auto& v : c) v /= lead;

    // Начальные приближения по спирали.
    std::vector<std::complex<double>> z(n);
    const std::complex<double> base(0.4, 0.9);
    for (int i = 0; i < n; ++i) {
        z[i] = std::pow(base, i + 1);
    }

    // Итерации Дюрана–Кернера.
    for (int iter = 0; iter < 2000; ++iter) {
        double maxDelta = 0.0;
        for (int i = 0; i < n; ++i) {
            std::complex<double> denom = 1.0;
            for (int j = 0; j < n; ++j) {
                if (i != j) denom *= (z[i] - z[j]);
            }
            if (std::abs(denom) < 1e-30) continue;
            const std::complex<double> delta = evalPoly(c, z[i]) / denom;
            z[i] -= delta;
            maxDelta = std::max(maxDelta, std::abs(delta));
        }
        if (maxDelta < 1e-12) break;
    }
    return z;
    }

    StabilityResult StabilityAnalyzer::analyze(const std::vector<double>& coeffs) {
        StabilityResult r;
        r.roots = findRoots(coeffs);
        if (r.roots.empty()) {
            r.stable = true;
            r.reason = "degenerate (constant) polynomial";
            return r;
        }
        for (const auto& root : r.roots) {
            r.maxMagnitude = std::max(r.maxMagnitude, std::abs(root));
        }
        if (r.maxMagnitude < 1.0 - kTolerance) {
            r.stable = true;
            r.reason = "all roots inside unit circle";
        } else if (std::abs(r.maxMagnitude - 1.0) < kTolerance) {
            r.stable = false;
            r.reason = "roots on unit circle (marginally stable)";
        } else {
            r.stable = false;
            r.reason = "roots outside unit circle (unstable)";
        }
        return r;
    }

    StabilityResult StabilityAnalyzer::analyzeModel15(double a1, double a2, int k) {
        std::vector<double> coeffs;
        if (k == 0) {
            coeffs = {-(a1 + a2), 1.0};                 // z − (a1+a2)
        } else {
            coeffs.assign(static_cast<std::size_t>(k) + 2, 0.0);
            coeffs[0]       = -a2;                      // z^0
            coeffs[k]       = -a1;                      // z^k
            coeffs[k + 1]   =  1.0;                     // z^{k+1}
        }
        auto r = analyze(coeffs);
        r.reason = "Model 1.5: " + r.reason;
        return r;
    }

    StabilityResult StabilityAnalyzer::analyzeModel27(double a) {
        auto r = analyze({-a, 1.0});                    // z − a
        r.reason = "Model 2.7: " + r.reason;
        return r;
    }

    StabilityResult StabilityAnalyzer::analyzeModel39() {
        auto r = analyze({-1.0, 1.0});                  // z − 1
        r.reason = "Model 3.9: " + r.reason;
        return r;
    }
