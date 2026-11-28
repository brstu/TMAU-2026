#include "stability.h"
#include "models.h"

#include <cmath>
#include <stdexcept>
#include <iostream>

StabilityStatus checkStability(double a1) {
    // Проверка корректности коэффициента
    if (!std::isfinite(a1)) {
        throw std::invalid_argument("Coefficient a1 must be finite.");
    }

    // Корень характеристического уравнения: z = a1
    // Устойчивость определяется положением корня на Z-плоскости
    if (std::abs(a1) < 1.0) {
        return StabilityStatus::Stable;
    }

    if (std::abs(a1) == 1.0) {
        return StabilityStatus::Boundary;
    }

    return StabilityStatus::Unstable;
}

std::string stabilityMessage(StabilityStatus status) {
    switch (status) {
        case StabilityStatus::Stable:
            return "Model 1.10 is asymptotically stable.";

        case StabilityStatus::Boundary:
            return "Model 1.10 is on the stability boundary.";

        case StabilityStatus::Unstable:
            return "Warning: Model 1.10 is unstable!";
    }

    return "Unknown stability status.";
}

StabilityStatus checkModelBeforeSimulation(const Model110& model) {
    StabilityStatus status = checkStability(model.getA1());

    std::cout << stabilityMessage(status) << std::endl;

    return status;
}