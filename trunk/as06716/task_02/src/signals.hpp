#pragma once

#include <memory>
#include <string>


class InputSignal {
public:
    virtual ~InputSignal() = default;

    virtual double value(int tau) const = 0;

    virtual std::string name() const = 0;
};


class StepSignal : public InputSignal {
public:
    double value(int tau) const override;

    std::string name() const override { return "Step"; }
};


class ImpulseSignal : public InputSignal {
public:
    double value(int tau) const override;

    std::string name() const override { return "Impulse"; }
};


class HarmonicSignal : public InputSignal {
public:
    double value(int tau) const override;

    std::string name() const override { return "Harmonic"; }
};


std::unique_ptr<InputSignal> makeSignal(int choice);
