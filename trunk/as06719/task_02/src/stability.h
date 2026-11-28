
#ifndef STABILITY_H
#define STABILITY_H

#include <string>

class Model110;

enum class StabilityStatus {
    Stable,
    Boundary,
    Unstable
};

StabilityStatus checkStability(double a1);
std::string stabilityMessage(StabilityStatus status);

// Проверяет устойчивость модели перед запуском моделирования.
StabilityStatus checkModelBeforeSimulation(const Model110& model);

#endif