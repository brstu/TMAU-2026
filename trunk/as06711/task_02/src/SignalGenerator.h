#pragma once
#include "ISignalGenerator.h"
#include <cmath>

// Типы входных воздействий из ЛР №1 (вариант 19).
enum class InputType {
    STEP,       // ступенчатый сигнал
    IMPULSE,    // импульс в момент tau = 0
    HARMONIC    // гармонический сигнал sin(tau)
};

class SignalGenerator : public ISignalGenerator {
public:
    explicit SignalGenerator(InputType type = InputType::STEP, double amplitude = 1.0)
        : type_(type), amplitude_(amplitude) {}

    double get(double tau) const override {
        switch (type_) {
        case InputType::STEP:     return amplitude_;
        case InputType::IMPULSE:  return (tau == 0.0) ? amplitude_ : 0.0;
        case InputType::HARMONIC: return amplitude_ * std::sin(tau);
        default:                  return 0.0;
        }
    }

private:
    InputType type_;
    double amplitude_;
};
