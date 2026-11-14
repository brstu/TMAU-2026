#include "stability_analyzer.h"

#include <cmath>
#include <iomanip>
#include <sstream>

namespace {
constexpr double kBoundaryTolerance = 1e-12;
}

StabilityReport StabilityAnalyzer::analyzeModel12(
    double a,
    std::size_t inputDelay
) {
    StabilityReport report;
    report.coefficientA = a;
    report.inputDelay = inputDelay;
    report.characteristicRoot = {a, 0.0};

    if (!std::isfinite(a) || inputDelay < 1) {
        report.status = StabilityStatus::InvalidParameters;
        return report;
    }

    const double modulus = std::abs(report.characteristicRoot);

    if (modulus < 1.0 - kBoundaryTolerance) {
        report.status = StabilityStatus::AsymptoticallyStable;
    } else if (modulus <= 1.0 + kBoundaryTolerance) {
        report.status = StabilityStatus::MarginallyStable;
    } else {
        report.status = StabilityStatus::Unstable;
    }

    return report;
}

bool StabilityReport::isAsymptoticallyStable() const {
    return status == StabilityStatus::AsymptoticallyStable;
}

std::string StabilityReport::message() const {
    std::ostringstream out;
    out << std::setprecision(8);
    out << "Проверка устойчивости Model 1.2. "
        << "Характеристическое уравнение: z - a = 0; корень z = "
        << characteristicRoot.real() << ". "
        << "Задержка входа k = " << inputDelay
        << " не изменяет положение корня. Критерий асимптотической "
        << "устойчивости: |z| < 1 (|a| < 1). ";

    switch (status) {
    case StabilityStatus::AsymptoticallyStable:
        out << "Условие выполнено: свободная составляющая затухает.";
        break;
    case StabilityStatus::MarginallyStable:
        out << "ПРЕДУПРЕЖДЕНИЕ: |z| = 1. Система находится на границе "
            << "устойчивости и не является асимптотически устойчивой.";
        break;
    case StabilityStatus::Unstable:
        out << "ПРЕДУПРЕЖДЕНИЕ: |z| > 1. Система неустойчива; "
            << "свободная составляющая возрастает.";
        break;
    case StabilityStatus::InvalidParameters:
        out << "ПРЕДУПРЕЖДЕНИЕ: недопустимые параметры анализа устойчивости.";
        break;
    }

    return out.str();
}
