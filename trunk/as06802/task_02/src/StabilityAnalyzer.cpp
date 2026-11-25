#include "StabilityAnalyzer.h"

#include <cmath>
#include <ostream>
#include <stdexcept>

namespace var17 {

bool PoleReport::isConvergent() const {
    return mode == PoleMode::AsymptoticallyStable;
}

PoleReport inspectLinearStability(double a) {
    if (!std::isfinite(a)) {
        throw std::invalid_argument("Параметр a должен быть конечным вещественным числом.");
    }

    const double radius = std::abs(a);
    if (radius < 1.0) {
        return {a, PoleMode::AsymptoticallyStable};
    }
    if (radius == 1.0) {
        return {a, PoleMode::BoundaryMarginal};
    }
    return {a, PoleMode::DivergentUnstable};
}

void printStabilityAlert(std::ostream& out, const PoleReport& report) {
    switch (report.mode) {
        case PoleMode::AsymptoticallyStable:
            return;
        case PoleMode::BoundaryMarginal:
            out << "Внимание: система находится на границе устойчивости (полюс z = "
                << report.poleLocation << ", |z| = 1). Затухание отсутствует.\n";
            return;
        case PoleMode::DivergentUnstable:
            out << "Внимание: линейная модель неустойчива (полюс z = "
                << report.poleLocation << ", |z| > 1). Траектория может неограниченно расти.\n";
            return;
    }
}

} // namespace var17
