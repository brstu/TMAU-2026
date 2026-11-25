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

struct StepContext {
    double y;
    double u;
    double uPrev2;
    double dt;
};

enum class InputType { Step = 1, Impulse = 2, Harmonic = 3 };
using InputSignal = std::function<double(int)>;
InputSignal makeInputSignal(InputType type);

class Model {
public:
    virtual ~Model();
    virtual double next(const StepContext& ctx) const = 0;
    virtual std::string_view name() const = 0;

    std::vector<Sample> simulate(int steps, double y0, double dt,
                                 const InputSignal& signal) const;
};

/// Linear discrete model with distributed control window (Variant 20, 1.10).
class DistributedControlWindow final : public Model {
public:
    DistributedControlWindow(double a1, double b1, double b2);
    double next(const StepContext& ctx) const override;
    std::string_view name() const override;

private:
    double a1_;
    double b1_;
    double b2_;
};

/// Nonlinear discrete model with logarithmic interaction (Variant 20, 2.4).
class LogarithmicInteraction final : public Model {
public:
    LogarithmicInteraction(double a, double b);
    double next(const StepContext& ctx) const override;
    std::string_view name() const override;

private:
    double a_;
    double b_;
};

/// Continuous linear-quadratic decay solved via Euler method (Variant 20, 3.8).
class CombinedQuadraticDecay final : public Model {
public:
    CombinedQuadraticDecay(double a1, double a2, double b);
    double next(const StepContext& ctx) const override;
    std::string_view name() const override;

private:
    double a1_;
    double a2_;
    double b_;
};

enum class Stability { Stable, Marginal, Unstable };

struct StabilityResult {
    double root;
    Stability classification;

    bool isAsymptoticallyStable() const;
};

StabilityResult analyzeDistributedControl(double a1);
void writeStabilityWarning(std::ostream& output, StabilityResult result);

} // namespace task02
