#include "InputSignal.h"
#include <cmath>
#include <stdexcept>

double StepSignal::value(int, double) const {
    return 1.0;
}

double ImpulseSignal::value(int step, double) const {
    return step == 0 ? 1.0 : 0.0;
}

double HarmonicSignal::value(int, double time) const {
    return std::sin(time);
}

std::unique_ptr<Signal> makeSignal(int type) {
    switch (type) {
        case 1: return std::make_unique<StepSignal>();
        case 2: return std::make_unique<ImpulseSignal>();
        case 3: return std::make_unique<HarmonicSignal>();
        default: throw std::invalid_argument("Unknown signal type");
    }
}
