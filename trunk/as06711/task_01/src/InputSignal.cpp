#include "InputSignal.h"
#include <cmath>

namespace {

    // Отдельные обработчики сигналов — вместо одного switch
    inline double stepSignal(int /*time*/) {
        return 1.0;
    }

    inline double impulseSignal(int time) {
        return (time == 0) ? 1.0 : 0.0;
    }

    inline double harmonicSignal(int time) {
        return std::sin(static_cast<double>(time));
    }

    inline double fallbackSignal(int /*time*/) {
        return 0.0;
    }

} // namespace

double getInputValue(InputType signalKind, int timeIndex) {
    switch (signalKind) {
    case InputType::STEP:     return stepSignal(timeIndex);
    case InputType::IMPULSE:  return impulseSignal(timeIndex);
    case InputType::HARMONIC: return harmonicSignal(timeIndex);
    default:                  return fallbackSignal(timeIndex);
    }
}