#include "StabilityAnalyzer.h"

#include <cmath>
#include <ostream>
#include <stdexcept>

namespace task02 {

bool StabilityResult::isAsymptoticallyStable() const {
    return classification == Stability::Stable;
}

StabilityResult analyzeDelayedControl(double a) {
    if (!std::isfinite(a)) {
        throw std::invalid_argument("Коэффициент a должен быть конечным.");
    }

    // The homogeneous equation is y(k+1) = a*y(k), so its pole is z = a.
    const double modulus = std::abs(a);
    if (modulus < 1.0) {
        return {a, Stability::Stable};
    }
    if (modulus == 1.0) {
        return {a, Stability::Marginal};
    }
    return {a, Stability::Unstable};
}

void writeStabilityWarning(std::ostream& output, StabilityResult result) {
    switch (result.classification) {
        case Stability::Stable:
            return;
        case Stability::Marginal:
            output << "Предупреждение: граница устойчивости (z = " << result.root
                   << ", |z| = 1). Свободный отклик не затухает.\n";
            return;
        case Stability::Unstable:
            output << "Предупреждение: модель неустойчива (z = " << result.root
                   << ", |z| > 1). Возможна расходимость.\n";
            return;
    }
}

} // namespace task02
