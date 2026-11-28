#include "StabilityAnalyzer.h"
#include <cmath>

bool StabilityAnalyzer::isLinearModelStable(double a) {
    return std::abs(a) < 1.0;
}

std::string StabilityAnalyzer::getWarningMessage(double a) {
    if (!isLinearModelStable(a)) {
        return "ВНИМАНИЕ: Модель неустойчива! Корень |z| >= 1 (|a| >= 1). Процесс будет расходящимся.";
    }
    return "";
}