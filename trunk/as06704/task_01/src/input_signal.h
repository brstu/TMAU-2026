#ifndef INPUT_SIGNAL_H
#define INPUT_SIGNAL_H

#include <cstddef>
#include <string>

class InputSignal {
public:
    virtual ~InputSignal() = default;
    virtual double value(std::size_t tau) const = 0;
    virtual std::string name() const = 0;
};

class StepSignal final : public InputSignal {
public:
    double value(std::size_t tau) const override;
    std::string name() const override;
};

class ImpulseSignal final : public InputSignal {
public:
    double value(std::size_t tau) const override;
    std::string name() const override;
};

class HarmonicSignal final : public InputSignal {
public:
    double value(std::size_t tau) const override;
    std::string name() const override;
};

#endif
