#include "StabilityAnalyzer.h"

#include <cmath>
#include <ostream>
#include <stdexcept>

namespace var16 {

bool JuryReport::isStable() const {
    return zone == StabilityZone::AsymptoticallyStable;
}

JuryReport evaluateThirdOrderStability(double a1, double a2, double a3) {
    if (!std::isfinite(a1) || !std::isfinite(a2) || !std::isfinite(a3)) {
        throw std::invalid_argument("Коэффициенты устойчивости a1, a2, a3 должны быть конечными.");
    }

    // Характеристический полином P(z) = z^3 - a1*z^2 - a2*z - a3.
    const double pOne = 1.0 - a1 - a2 - a3;
    const double negPMinusOne = 1.0 + a1 - a2 + a3;
    const double modA3 = std::abs(a3);
    const double margin = std::abs(1.0 - a3 * a3) - std::abs(a2 + a1 * a3);

    if (pOne > 0.0 && negPMinusOne > 0.0 && modA3 < 1.0 && margin > 0.0) {
        return {StabilityZone::AsymptoticallyStable, pOne, negPMinusOne, modA3, margin};
    }

    if (pOne >= 0.0 && negPMinusOne >= 0.0 && modA3 <= 1.0 && margin >= 0.0) {
        return {StabilityZone::CriticalBoundary, pOne, negPMinusOne, modA3, margin};
    }

    return {StabilityZone::UnstableDivergent, pOne, negPMinusOne, modA3, margin};
}

void reportStabilityStatus(std::ostream& stream, const JuryReport& report) {
    if (report.zone == StabilityZone::AsymptoticallyStable) {
        return;
    }
    if (report.zone == StabilityZone::CriticalBoundary) {
        stream << "Предупреждение: система 3-го порядка находится на границе устойчивости "
               << "(P(1) = " << report.pAtOne
               << ", -P(-1) = " << report.negPAtMinusOne
               << ", |a3| = " << report.absA3 << ").\n";
        return;
    }
    stream << "Предупреждение: система 3-го порядка неустойчива на Z-плоскости "
           << "(P(1) = " << report.pAtOne
           << ", -P(-1) = " << report.negPAtMinusOne
           << ", |a3| = " << report.absA3 << "). Возможна расходимость.\n";
}

} // namespace var16
