#include "InputSignal.h"
#include <cmath>

namespace {

    inline double stepSignal(int /*time*/) {
        return 1.0;
    }

    inline double impulseSignal(int time) {
        return (time == 0) ? 1.0 : 0.0;
    }

    inline double harmonicSignal(int time) {
        return std::sin(static_cast<double>(time));
    }

} // namespace

double getInputValue(InputType signalKind, int timeIndex) {
    switch (signalKind) {
    case InputType::STEP:     return stepSignal(timeIndex);
    case InputType::IMPULSE:  return impulseSignal(timeIndex);
    case InputType::HARMONIC: return harmonicSignal(timeIndex);
    default:                  return 0.0;
    }
}
