#include "InputSignal.h"
#include <cmath>

double StepSignal::valueAt(int /*step*/) const {
    return level_;
}

double ImpulseSignal::valueAt(int step) const {
    return step == 0 ? height_ : 0.0;
}

double HarmonicSignal::valueAt(int step) const {
    return amplitude_ * std::sin(static_cast<double>(step));
}

std::vector<std::unique_ptr<InputSignal>> makeSignals(int menuChoice) {
    std::vector<std::unique_ptr<InputSignal>> result;
    const bool all = (menuChoice == 4);

    if (all || menuChoice == 1) {
        result.push_back(std::make_unique<StepSignal>());
    }
    if (all || menuChoice == 2) {
        result.push_back(std::make_unique<ImpulseSignal>());
    }
    if (all || menuChoice == 3) {
        result.push_back(std::make_unique<HarmonicSignal>());
    }
    return result;
}
