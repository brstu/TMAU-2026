#pragma once

#include <cmath>
#include <string>

class InputSignal {
public:
    virtual ~InputSignal() = default;
    virtual std::string name() const = 0;
    virtual double valueAt(int tau) const = 0;
};

class StepSignal final : public InputSignal {
public:
    std::string name() const override { return "Ступенчатый (uτ = 1)"; }
    double valueAt(int) const override { return 1.0; }
};

class ImpulseSignal final : public InputSignal {
public:
    std::string name() const override { return "Импульсный (u0 = 1, далее 0)"; }
    double valueAt(int tau) const override { return tau == 0 ? 1.0 : 0.0; }
};

class HarmonicSignal final : public InputSignal {
public:
    std::string name() const override { return "Гармонический (uτ = sin(τ))"; }
    double valueAt(int tau) const override { return std::sin(static_cast<double>(tau)); }
};
