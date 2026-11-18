#include "StabilityAnalyzer.h"

#include <algorithm>
#include <cmath>
#include <sstream>

namespace {

    using Complex = std::complex<double>;

    // Значение полинома в точке по схеме Горнера
    Complex evaluate(const std::vector<double>& coeffs, Complex z) {
        Complex result = 0.0;
        for (double c : coeffs) {
            result = result * z + c;
        }
        return result;
    }

    bool allFinite(const std::vector<double>& values) {
        return std::all_of(values.begin(), values.end(),
                           [](double v) { return std::isfinite(v); });
    }

    double maxModulus(const std::vector<Complex>& roots) {
        double result = 0.0;
        for (const auto& r : roots) {
            result = std::max(result, std::abs(r));
        }
        return result;
    }

    // Общий анализ по коэффициентам характеристического полинома
    StabilityResult analyze(const std::vector<double>& coeffs, std::string equation) {
        StabilityResult result;
        result.equation = std::move(equation);

        if (!allFinite(coeffs)) {
            // NaN или бесконечность в коэффициентах - считаем систему неустойчивой
            result.verdict = Stability::Unstable;
            result.maxModulus = INFINITY;
            return result;
        }

        result.maxModulus = maxModulus(polynomialRoots(coeffs));
        result.verdict = classify(result.maxModulus);
        return result;
    }

    // Произведение (z_i - z_j) по всем j != i - знаменатель поправки Дюрана-Кернера
    Complex productOfDifferences(const std::vector<Complex>& roots, size_t i) {
        Complex product = 1.0;
        for (size_t j = 0; j < roots.size(); ++j) {
            product *= (j == i) ? Complex(1.0) : (roots[i] - roots[j]);
        }
        return product;
    }

    std::string number(double value) {
        std::ostringstream out;
        out << value;
        return out.str();
    }

} // namespace

std::vector<std::complex<double>> polynomialRoots(const std::vector<double>& coeffs) {
    const size_t n = coeffs.size() < 2 ? 0 : coeffs.size() - 1;
    std::vector<Complex> roots(n);

    // Начальные приближения - степени комплексного числа, не лежащего на осях
    const Complex seed(0.4, 0.9);
    for (size_t i = 0; i < n; ++i) {
        roots[i] = std::pow(seed, static_cast<int>(i));
    }

    const int maxIterations = 5000;
    const double tolerance = 1e-14;
    for (int iter = 0; iter < maxIterations; ++iter) {
        double maxShift = 0.0;
        for (size_t i = 0; i < n; ++i) {
            const Complex shift = evaluate(coeffs, roots[i]) / productOfDifferences(roots, i);
            roots[i] -= shift;
            maxShift = std::max(maxShift, std::abs(shift));
        }
        if (maxShift < tolerance) {
            break;
        }
    }
    return roots;
}

Stability classify(double maxModulus) {
    if (std::isnan(maxModulus) || maxModulus > 1.0 + kBoundaryTolerance) {
        return Stability::Unstable;  // сюда же попадает NaN
    }
    if (maxModulus < 1.0 - kBoundaryTolerance) {
        return Stability::Stable;
    }
    return Stability::Boundary;
}

StabilityResult analyzeModel1_5(double a1, double a2, int k) {
    const int delay = k < 1 ? 1 : k;  // как в конструкторе Model1_5

    std::vector coeffs(static_cast<size_t>(delay) + 2, 0.0);
    coeffs[0] = 1.0;
    coeffs[1] = -a1;
    coeffs[static_cast<size_t>(delay) + 1] -= a2;

    const std::string equation = "z^" + std::to_string(delay + 1) + " - (" + number(a1) +
                                 ")*z^" + std::to_string(delay) + " - (" + number(a2) + ") = 0";
    return analyze(coeffs, equation);
}

StabilityResult analyzeModel2_9(double a) {
    return analyze({ 1.0, -a }, "z - (" + number(a) + ") = 0");
}

StabilityResult analyzeModel3_3(double a, double dt) {
    const double root = 1.0 - a * dt;
    return analyze({ 1.0, -root },
                   "z - (1 - " + number(a) + "*" + number(dt) + ") = 0");
}

bool juryCriterionK1(double a1, double a2) {
    return std::fabs(a2) < 1.0 && (1.0 - a1 - a2) > 0.0 && (1.0 + a1 - a2) > 0.0;
}

bool isStable(const StabilityResult& result) {
    return result.verdict == Stability::Stable;
}

bool warnIfUnstable(const std::string& modelName, const StabilityResult& result, std::ostream& out) {
    if (isStable(result)) {
        return false;
    }

    out << "[WARNING] " << modelName << ": " << result.equation
        << ", max |z| = " << result.maxModulus << ". ";
    if (result.verdict == Stability::Boundary) {
        out << "Root on the unit circle |z| = 1, the process does NOT decay.\n";
    } else {
        out << "Root outside the unit circle, the process DIVERGES!\n";
    }
    return true;
}

void requireStable(const std::string& modelName, const StabilityResult& result) {
    if (!isStable(result)) {
        throw UnstableSystemError(modelName + " is not stable: max |z| = " +
                                  number(result.maxModulus));
    }
}
