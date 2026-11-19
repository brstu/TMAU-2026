#include "stability.hpp"

#include <algorithm>
#include <cmath>
#include <ostream>

namespace stability {

namespace {

constexpr int MAX_ITERATIONS = 1000;
constexpr double TOLERANCE = 1e-14;

bool allFinite(double a1, double a2, double a3) {
    return std::isfinite(a1) && std::isfinite(a2) && std::isfinite(a3);
}

// z^3 + c2*z^2 + c1*z + c0
Complex evaluate(const Complex& z, double c2, double c1, double c0) {
    return ((z + c2) * z + c1) * z + c0;
}

}  // namespace


std::vector<Complex> characteristicRoots(double a1, double a2, double a3) {
    const double c2 = -a1;
    const double c1 = -a2;
    const double c0 = -a3;

    const Complex seed(0.4, 0.9);
    std::vector<Complex> roots = {Complex(1.0, 0.0), seed, seed * seed};

    for (int iteration = 0; iteration < MAX_ITERATIONS; ++iteration) {
        double delta = 0.0;
        for (std::size_t i = 0; i < roots.size(); ++i) {
            Complex denominator(1.0, 0.0);
            for (std::size_t j = 0; j < roots.size(); ++j) {
                if (j != i) {
                    denominator *= roots[i] - roots[j];
                }
            }
            const Complex correction = evaluate(roots[i], c2, c1, c0) / denominator;
            roots[i] -= correction;
            delta = std::max(delta, std::abs(correction));
        }
        if (delta < TOLERANCE) {
            break;
        }
    }

    return roots;
}

bool isStable(double a1, double a2, double a3) {
    if (!allFinite(a1, a2, a3)) {
        return false;
    }

    // P(z) = z^3 + c2*z^2 + c1*z + c0
    const double c2 = -a1;
    const double c1 = -a2;
    const double c0 = -a3;

    const double atPlusOne = 1.0 + c2 + c1 + c0;       // P(1)  > 0
    const double atMinusOne = -1.0 + c2 - c1 + c0;     // P(-1) < 0
    if (!(atPlusOne > 0.0) || !(atMinusOne < 0.0)) {
        return false;
    }
    if (!(std::abs(c0) < 1.0)) {                       // |c0| < 1
        return false;
    }

    const double b0 = c0 * c0 - 1.0;
    const double b2 = c0 * c2 - c1;

    return std::abs(b0) > std::abs(b2);                // |b0| > |b2|
}

Report analyze(double a1, double a2, double a3) {
    Report report;
    report.stable = isStable(a1, a2, a3);
    if (!allFinite(a1, a2, a3)) {
        report.spectralRadius = INFINITY;

        return report;
    }

    report.roots = characteristicRoots(a1, a2, a3);
    for (const Complex& root : report.roots) {
        report.spectralRadius = std::max(report.spectralRadius, std::abs(root));
    }

    return report;
}

bool warnIfUnstable(double a1, double a2, double a3, std::ostream& out) {
    const Report report = analyze(a1, a2, a3);
    if (!report.stable) {
        out << "\nВНИМАНИЕ: модель 1.6 неустойчива (max |z| = " << report.spectralRadius
            << ", нужно < 1). Процесс будет расходиться или не затухать.\n";
    }

    return report.stable;
}

}  // namespace stability
