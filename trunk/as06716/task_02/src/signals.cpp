#include "signals.hpp"

#include <cmath>


double StepSignal::value(int /*tau*/) const {
    return 1.0;
}

double ImpulseSignal::value(int tau) const {
    return tau == 0 ? 1.0 : 0.0;
}

double HarmonicSignal::value(int tau) const {
    return std::sin(static_cast<double>(tau));
}

std::unique_ptr<InputSignal> makeSignal(int choice) {
    switch (choice) {
        case 1: return std::make_unique<StepSignal>();
        case 2: return std::make_unique<ImpulseSignal>();
        case 3: return std::make_unique<HarmonicSignal>();
        default: return nullptr;
    }
}
