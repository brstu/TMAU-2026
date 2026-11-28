#include "InputSignal.h"

#include <cmath>
#include <stdexcept>

double StepInput::valueAt(int /*tau*/) const {
    return 1.0;
}

std::string StepInput::name() const {
    return "Step";
}

double ImpulseInput::valueAt(int tau) const {
    return tau == 0 ? 1.0 : 0.0;
}

std::string ImpulseInput::name() const {
    return "Impulse";
}

double HarmonicInput::valueAt(int tau) const {
    return std::sin(static_cast<double>(tau));
}

std::string HarmonicInput::name() const {
    return "Harmonic";
}

std::unique_ptr<InputSignal> createInputSignal(InputKind kind) {
    switch (kind) {
    case InputKind::Step:
        return std::make_unique<StepInput>();
    case InputKind::Impulse:
        return std::make_unique<ImpulseInput>();
    case InputKind::Harmonic:
        return std::make_unique<HarmonicInput>();
    }
    throw std::invalid_argument("Unknown input signal type");
}
