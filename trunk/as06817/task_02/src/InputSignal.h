#pragma once

#include "ISignalSource.h"

enum class SignalType {
    Step = 1,
    Impulse = 2,
    Harmonic = 3
};

// Формирователь входных воздействий:
//   Step     : u(tau) = A
//   Impulse  : u(0) = A, u(tau > 0) = 0
//   Harmonic : u(tau) = A * sin(w * tau + phi)
class InputSignal final : public ISignalSource {
public:
    explicit InputSignal(SignalType type,
                         double amplitude = 1.0,
                         double frequency = 1.0,
                         double phase = 0.0);

    double value(int tau) const override;

    SignalType type() const { return type_; }
    double amplitude() const { return amplitude_; }
    double frequency() const { return frequency_; }
    double phase() const { return phase_; }

private:
    SignalType type_;
    double amplitude_;
    double frequency_;
    double phase_;
};
