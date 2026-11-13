#include "StabilityAnalyzer.h"

#include "IModel.h"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>

namespace {

constexpr double kCoefficientEpsilon = 1e-15;
constexpr double kDenominatorEpsilon = 1e-300;
constexpr double kConvergenceEpsilon = 1e-14;
constexpr int kMaxIterations = 5000;

std::vector<double> normalizeCoefficients(const std::vector<double>& coefficients) {
    const auto first = std::find_if(
        coefficients.begin(),
        coefficients.end(),
        [](double value) { return std::abs(value) > kCoefficientEpsilon; });

    return std::vector<double>(first, coefficients.end());
}

std::complex<double> evaluatePolynomial(const std::vector<double>& coefficients,
                                        const std::complex<double>& z) {
    std::complex<double> value(0.0, 0.0);
    for (double coefficient : coefficients) {
        value = value * z + coefficient;
    }
    return value;
}

// Приводит полином к виду z^n + a1*z^(n-1) + ... + an.
std::vector<double> toMonic(const std::vector<double>& coefficients) {
    std::vector<double> monic(coefficients.size());
    for (std::size_t i = 0; i < coefficients.size(); ++i) {
        monic[i] = coefficients[i] / coefficients[0];
    }
    return monic;
}

// Начальное приближение Дюрана - Кернера.
std::vector<std::complex<double>> initialApproximation(int degree) {
    std::vector<std::complex<double>> roots(static_cast<std::size_t>(degree));
    const std::complex<double> seed(0.4, 0.9);
    std::complex<double> current(1.0, 0.0);

    for (int i = 0; i < degree; ++i) {
        roots[static_cast<std::size_t>(i)] = current;
        current *= seed;
    }

    return roots;
}

// Знаменатель шага Дюрана - Кернера: произведение (z_i - z_j), j != i.
std::complex<double> evaluateDenominator(
    const std::vector<std::complex<double>>& roots,
    std::size_t index) {
    std::complex<double> denominator(1.0, 0.0);

    for (std::size_t j = 0; j < roots.size(); ++j) {
        if (j != index) {
            denominator *= roots[index] - roots[j];
        }
    }

    return denominator;
}

std::vector<std::complex<double>> durandKerner(
    const std::vector<double>& monic) {
    const int degree = static_cast<int>(monic.size()) - 1;
    std::vector<std::complex<double>> roots = initialApproximation(degree);

    for (int iteration = 0; iteration < kMaxIterations; ++iteration) {
        double maxDelta = 0.0;

        for (int i = 0; i < degree; ++i) {
            const auto index = static_cast<std::size_t>(i);
            const std::complex<double> denominator =
                evaluateDenominator(roots, index);

            if (std::abs(denominator) < kDenominatorEpsilon) {
                continue;
            }

            const std::complex<double> delta =
                evaluatePolynomial(monic, roots[index]) / denominator;

            roots[index] -= delta;
            maxDelta = std::max(maxDelta, std::abs(delta));
        }

        if (maxDelta < kConvergenceEpsilon) {
            break;
        }
    }

    return roots;
}

} // namespace

std::vector<std::complex<double>> StabilityAnalyzer::findRoots(
    const std::vector<double>& coefficients) {
    std::vector<double> normalized = normalizeCoefficients(coefficients);
    if (normalized.size() <= 1) {
        return {};
    }

    // Множитель z^k соответствует k корням в начале координат.
    int zeroRoots = 0;
    while (normalized.size() > 1
           && std::abs(normalized.back()) <= kCoefficientEpsilon) {
        normalized.pop_back();
        ++zeroRoots;
    }

    const int degree = static_cast<int>(normalized.size()) - 1;
    std::vector<std::complex<double>> roots;

    if (degree == 1) {
        roots.emplace_back(-normalized[1] / normalized[0], 0.0);
    } else if (degree >= 2) {
        roots = durandKerner(toMonic(normalized));
    }

    for (int i = 0; i < zeroRoots; ++i) {
        roots.emplace_back(0.0, 0.0);
    }

    return roots;
}

StabilityReport StabilityAnalyzer::analyze(
    const std::vector<double>& coefficients) {
    StabilityReport report;

    const std::vector<double> normalized = normalizeCoefficients(coefficients);
    if (normalized.empty()) {
        report.analyzed = false;
        report.stable = true;
        report.message = "Характеристическое уравнение не задано.";
        return report;
    }

    report.analyzed = true;

    if (normalized.size() == 1) {
        report.stable = true;
        report.message =
            "Уравнение не содержит динамических корней (нет переменной z).";
        return report;
    }

    report.roots = findRoots(normalized);
    report.maxMagnitude = 0.0;
    for (const auto& root : report.roots) {
        report.maxMagnitude = std::max(report.maxMagnitude, std::abs(root));
    }

    if (report.maxMagnitude < 1.0 - kTolerance) {
        report.stable = true;
        report.message =
            "Система устойчива: все корни лежат внутри единичной окружности "
            "(|z| < 1).";
    } else if (report.maxMagnitude <= 1.0 + kTolerance) {
        report.stable = false;
        report.marginal = true;
        report.message =
            "Система на границе устойчивости: |z| = 1, процесс не затухает.";
    } else {
        report.stable = false;
        report.message =
            "Система НЕУСТОЙЧИВА: |z| > 1, процесс расходится.";
    }

    return report;
}

StabilityReport StabilityAnalyzer::analyze(const IModel& model) {
    if (!model.isLinear()) {
        StabilityReport report;
        report.analyzed = false;
        report.stable = true;
        report.message =
            "Модель нелинейная: анализ устойчивости на Z-плоскости не применим.";
        return report;
    }

    const std::vector<double> polynomial = model.characteristicPolynomial();
    if (polynomial.empty()) {
        StabilityReport report;
        report.analyzed = false;
        report.stable = true;
        report.message = "Характеристическое уравнение не задано.";
        return report;
    }

    return analyze(polynomial);
}

std::string StabilityAnalyzer::buildWarning(
    const IModel& model,
    const StabilityReport& report) {
    std::ostringstream out;
    out << "[WARNING] " << model.name() << ": " << report.message;

    if (!report.roots.empty()) {
        out << " Корни характеристического уравнения: ";
        out << std::fixed << std::setprecision(6);
        for (std::size_t i = 0; i < report.roots.size(); ++i) {
            if (i > 0) {
                out << ", ";
            }
            out << "z" << (i + 1) << " = ("
                << report.roots[i].real() << ", "
                << report.roots[i].imag() << ")";
        }
        out << ". |z|max = " << report.maxMagnitude << ".";
    }

    return out.str();
}
