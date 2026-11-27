#include "InputSignal.h"

#include <cmath>

InputSignal::InputSignal(SignalType type,
                         double amplitude,
                         double frequency,
                         double phase)
    : type_(type),
      amplitude_(amplitude),
      frequency_(frequency),
      phase_(phase) {
}

double InputSignal::value(int tau) const {
    switch (type_) {
    case SignalType::Step:
        return amplitude_;

    case SignalType::Impulse:
        return tau == 0 ? amplitude_ : 0.0;

    case SignalType::Harmonic:
        return amplitude_ * std::sin(frequency_ * static_cast<double>(tau) + phase_);
    }

    return 0.0;
}
