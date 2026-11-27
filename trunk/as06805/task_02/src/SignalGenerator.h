#pragma once

#include "Model.h"

#include <cmath>
#include <string>

enum class SignalType {
    STEP,
    IMPULSE,
    HARMONIC
};

// Генератор типовых входных воздействий:
//   STEP     — u(tau) = A
//   IMPULSE  — u(0) = A, u(tau > 0) = 0
//   HARMONIC — u(tau) = A * sin(tau)
class SignalGenerator : public ISignalSource {
public:
    explicit SignalGenerator(SignalType type = SignalType::STEP, double amp = 1.0);

    double get(double tau) const override;

    SignalType getType() const noexcept { return currentType_; }
    double getAmplitude() const noexcept { return signalAmp_; }

    // Текстовое имя типа сигнала.
    static std::string typeName(SignalType type);

private:
    SignalType currentType_;
    double signalAmp_;
};
