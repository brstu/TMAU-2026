#pragma once

#include <memory>
#include <string>

enum class InputKind {
    Step,
    Impulse,
    Harmonic
};

class InputSignal {
public:
    virtual ~InputSignal() = default;
    virtual double valueAt(int tau) const = 0;
    virtual std::string name() const = 0;
};

class StepInput final : public InputSignal {
public:
    double valueAt(int tau) const override;
    std::string name() const override;
};

class ImpulseInput final : public InputSignal {
public:
    double valueAt(int tau) const override;
    std::string name() const override;
};

class HarmonicInput final : public InputSignal {
public:
    double valueAt(int tau) const override;
    std::string name() const override;
};

std::unique_ptr<InputSignal> createInputSignal(InputKind kind);
