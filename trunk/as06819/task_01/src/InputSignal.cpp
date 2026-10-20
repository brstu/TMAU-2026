#include "InputSignal.h"
#include <cmath>

double getInputValue(InputType type, int t) {
    switch (type) {
        case InputType::STEP:     return 1.0;
        case InputType::IMPULSE:  return (t == 0) ? 1.0 : 0.0;
        case InputType::HARMONIC: return std::sin(static_cast<double>(t));
        default:                  return 0.0;
    }
}
