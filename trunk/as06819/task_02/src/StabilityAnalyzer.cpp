#include "StabilityAnalyzer.h"

#include <cmath>
#include <ostream>
#include <stdexcept>

namespace task02 {

bool StabilityResult::isAsymptoticallyStable() const {
    return classification == Stability::Stable;
}

StabilityResult analyzeDistributedControl(double a1) {
    if (!std::isfinite(a1)) {
        throw std::invalid_argument("Коэффициент a1 должен быть конечным.");
    }

    // Свободное движение: y(tau+1) = a1*y(tau). Характеристический корень z = a1.
    const double modulus = std::abs(a1);
    if (modulus < 1.0) {
        return {a1, Stability::Stable};
    }
    if (modulus == 1.0) {
        return {a1, Stability::Marginal};
    }
    return {a1, Stability::Unstable};
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
                   << ", |z| > 1). Возможна расходимость процесса.\n";
            return;
    }
}

} // namespace task02
