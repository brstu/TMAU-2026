#include "input_signal.h"

#include <cmath>

double StepSignal::value(std::size_t) const {
    return 1.0;
}

std::string StepSignal::name() const {
    return "Ступенчатый сигнал: u(tau) = 1";
}

double ImpulseSignal::value(std::size_t tau) const {
    return tau == 0 ? 1.0 : 0.0;
}

std::string ImpulseSignal::name() const {
    return "Импульсный сигнал: u(0) = 1, далее 0";
}

double HarmonicSignal::value(std::size_t tau) const {
    return std::sin(static_cast<double>(tau));
}

std::string HarmonicSignal::name() const {
    return "Гармонический сигнал: u(tau) = sin(tau)";
}
