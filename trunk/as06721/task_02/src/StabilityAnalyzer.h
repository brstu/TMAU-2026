#pragma once

#include <functional>
#include <iosfwd>
#include <string_view>
#include <vector>

namespace task02 {

struct Sample {
    int step;
    double input;
    double output;
};

enum class InputType { Step = 1, Impulse = 2, Harmonic = 3 };
using InputSignal = std::function<double(int)>;
InputSignal makeInputSignal(InputType type);

class Model {
public:
    virtual ~Model();
    virtual double next(double y, double input, double dt) const = 0;
    virtual std::string_view name() const = 0;

    std::vector<Sample> simulate(int steps, double y0, double dt,
                                 const InputSignal& signal) const;

protected:
    virtual int inputDelay() const;
};

class DelayedControl final : public Model {
public:
    DelayedControl(double a, double b, int delay);
    double next(double y, double delayedInput, double dt) const override;
    std::string_view name() const override;

private:
    int inputDelay() const override;
    double a_;
    double b_;
    int delay_;
};

class HysteresisRelay final : public Model {
public:
    HysteresisRelay(double a, double b, double epsilon);
    double next(double y, double input, double dt) const override;
    std::string_view name() const override;

private:
    double a_;
    double b_;
    double epsilon_;
};

class HarmonicForce final : public Model {
public:
    explicit HarmonicForce(double b);
    double next(double y, double input, double dt) const override;
    std::string_view name() const override;

private:
    double b_;
};

enum class Stability { Stable, Marginal, Unstable };

struct StabilityResult {
    double root;
    Stability classification;

    bool isAsymptoticallyStable() const;
};

StabilityResult analyzeDelayedControl(double a);
void writeStabilityWarning(std::ostream& output, StabilityResult result);

} // namespace task02
