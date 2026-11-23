#include "InputSignal.h"
#include <cmath>

double InputSignal::getValue(SignalType type, int tau) {
    switch (type) {
    case SignalType::Step:
        return 1.0;

    case SignalType::Impulse:
        return tau == 0 ? 1.0 : 0.0;

    case SignalType::Harmonic:
        return std::sin(static_cast<double>(tau));

    default:
        return 0.0;
    }
}