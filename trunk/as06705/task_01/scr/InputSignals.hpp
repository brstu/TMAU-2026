#pragma once
#include <cmath>
#include <memory>

class ISignal {
public:
    virtual ~ISignal() = default;
    virtual double at(int tau) const = 0;
    virtual const char* id() const = 0;
};

class StepSignal final : public ISignal {
public:
    explicit StepSignal(double amp = 1.0) : amp_(amp) {}
    double at(int) const override { return amp_; }
    const char* id() const override { return "step"; }
private:
    double amp_;
};

class ImpulseSignal final : public ISignal {
public:
    explicit ImpulseSignal(double amp = 1.0) : amp_(amp) {}
    double at(int tau) const override { return tau == 0 ? amp_ : 0.0; }
    const char* id() const override { return "impulse"; }
private:
    double amp_;
};

class HarmonicSignal final : public ISignal {
public:
    explicit HarmonicSignal(double amp = 1.0) : amp_(amp) {}
    double at(int tau) const override {
        return amp_ * std::sin(static_cast<double>(tau));
    }
    const char* id() const override { return "harmonic"; }
private:
    double amp_;
};

inline std::unique_ptr<ISignal> makeSignal(int choice, double amp = 1.0) {
    switch (choice) {
    case 1: return std::make_unique<StepSignal>(amp);
    case 2: return std::make_unique<ImpulseSignal>(amp);
    case 3: return std::make_unique<HarmonicSignal>(amp);
    }
    return nullptr;
}