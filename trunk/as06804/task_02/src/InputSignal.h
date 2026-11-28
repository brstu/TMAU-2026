#ifndef INPUT_SIGNAL_H
#define INPUT_SIGNAL_H

#include <memory>

// Input action abstraction. Extracted from main.cpp so that simulations can
// be tested with Google Mock substitutes (see test/test_runner.cpp).
class Signal {
public:
    virtual ~Signal() = default;
    virtual double value(int step, double time) const = 0;
};

class StepSignal final : public Signal {
public:
    double value(int step, double time) const override;
};

class ImpulseSignal final : public Signal {
public:
    double value(int step, double time) const override;
};

class HarmonicSignal final : public Signal {
public:
    double value(int step, double time) const override;
};

// type: 1 - step, 2 - impulse, 3 - harmonic.
// Throws std::invalid_argument for an unknown type.
std::unique_ptr<Signal> makeSignal(int type);

#endif // INPUT_SIGNAL_H
