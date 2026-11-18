#include "StabilityAnalyzer.h"

#include <cmath>
#include <iomanip>
#include <sstream>
#include <stdexcept>

bool StabilityReport::isAsymptoticallyStable() const {
    return status == StabilityStatus::AsymptoticallyStable;
}

StabilityReport StabilityAnalyzer::analyze(double a1, double a2, double tolerance) {
    if (!std::isfinite(a1) || !std::isfinite(a2)) {
        throw std::invalid_argument("Stability coefficients must be finite");
    }
    if (tolerance <= 0.0 || !std::isfinite(tolerance)) {
        throw std::invalid_argument("Tolerance must be a positive finite number");
    }

    const std::complex<double> discriminant(a1 * a1 + 4.0 * a2, 0.0);
    const std::complex<double> rootPart = std::sqrt(discriminant);
    const std::complex<double> root1 = (a1 + rootPart) / 2.0;
    const std::complex<double> root2 = (a1 - rootPart) / 2.0;

    const double abs1 = std::abs(root1);
    const double abs2 = std::abs(root2);

    StabilityStatus status;
    if (abs1 < 1.0 - tolerance && abs2 < 1.0 - tolerance) {
        status = StabilityStatus::AsymptoticallyStable;
    } else if (abs1 <= 1.0 + tolerance && abs2 <= 1.0 + tolerance &&
               (std::abs(abs1 - 1.0) <= tolerance ||
                std::abs(abs2 - 1.0) <= tolerance)) {
        status = StabilityStatus::MarginallyStable;
    } else {
        status = StabilityStatus::Unstable;
    }

    return {root1, root2, status};
}

std::string StabilityAnalyzer::statusText(StabilityStatus status) {
    switch (status) {
    case StabilityStatus::AsymptoticallyStable:
        return "Асимптотически устойчива";
    case StabilityStatus::MarginallyStable:
        return "На границе устойчивости (не является асимптотически устойчивой)";
    case StabilityStatus::Unstable:
        return "Неустойчива";
    }
    return "Неизвестный статус";
}

std::string StabilityAnalyzer::formatReport(const StabilityReport& report) {
    std::ostringstream out;
    out << std::fixed << std::setprecision(6);
    out << "Характеристическое уравнение: z^2 - a1*z - a2 = 0\n";
    out << "Корень z1 = " << report.root1
        << ", |z1| = " << std::abs(report.root1) << '\n';
    out << "Корень z2 = " << report.root2
        << ", |z2| = " << std::abs(report.root2) << '\n';
    out << "Критерий: оба корня должны находиться внутри единичной окружности.\n";
    out << "Результат: " << statusText(report.status) << ".\n";
    if (!report.isAsymptoticallyStable()) {
        out << "ПРЕДУПРЕЖДЕНИЕ: заданные коэффициенты могут привести к неустойчивому или незатухающему процессу.\n";
    }
    return out.str();
}
