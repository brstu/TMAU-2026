#include "StabilityAnalyzer.h"

#include "IModel.h"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>

namespace {

std::vector<double> normalizeCoefficients(const std::vector<double>& coefficients) {
    const auto first = std::find_if(
        coefficients.begin(),
        coefficients.end(),
        [](double value) { return std::abs(value) > 1e-15; });

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

} // namespace

std::vector<std::complex<double>> StabilityAnalyzer::findRoots(
    const std::vector<double>& coefficients) {
    std::vector<double> normalized = normalizeCoefficients(coefficients);
    if (normalized.size() <= 1) {
        return {};
    }

    // Множитель z^k соответствует k корням в начале координат.
    int zeroRoots = 0;
    while (normalized.size() > 1 && std::abs(normalized.back()) <= 1e-15) {
        normalized.pop_back();
        ++zeroRoots;
    }

    const int degree = static_cast<int>(normalized.size()) - 1;
    std::vector<std::complex<double>> roots;
    roots.reserve(static_cast<std::size_t>(degree + zeroRoots));

    if (degree == 1) {
        roots.emplace_back(-normalized[1] / normalized[0], 0.0);
    } else if (degree >= 2) {
        std::vector<double> monic(normalized.size());
        for (std::size_t i = 0; i < normalized.size(); ++i) {
            monic[i] = normalized[i] / normalized[0];
        }

        // Начальное приближение Дюрана - Кернера.
        std::vector<std::complex<double>> z(static_cast<std::size_t>(degree));
        const std::complex<double> seed(0.4, 0.9);
        std::complex<double> current(1.0, 0.0);
        for (int i = 0; i < degree; ++i) {
            z[static_cast<std::size_t>(i)] = current;
            current *= seed;
        }

        for (int iteration = 0; iteration < 5000; ++iteration) {
            double maxDelta = 0.0;
            for (int i = 0; i < degree; ++i) {
                std::complex<double> denominator(1.0, 0.0);
                for (int j = 0; j < degree; ++j) {
                    if (i != j) {
                        denominator *= (z[static_cast<std::size_t>(i)]
                            - z[static_cast<std::size_t>(j)]);
                    }
                }

                if (std::abs(denominator) < 1e-300) {
                    continue;
                }

                const std::complex<double> delta =
                    evaluatePolynomial(monic, z[static_cast<std::size_t>(i)])
                    / denominator;

                z[static_cast<std::size_t>(i)] -= delta;
                maxDelta = std::max(maxDelta, std::abs(delta));
            }

            if (maxDelta < 1e-14) {
                break;
            }
        }

        roots = z;
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
