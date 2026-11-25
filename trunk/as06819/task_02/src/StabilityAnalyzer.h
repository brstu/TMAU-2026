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

// Model 1.10: y(tau+1) = a1*y(tau) + b1*u(tau) + b2*u(tau-2)
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

// Model 2.4: y(tau+1) = a*y(tau)*cos(y(tau)) + b*ln(1 + |u(tau)|)
class LogarithmicInteraction final : public Model {
public:
    LogarithmicInteraction(double a, double b);
    double next(const StepContext& ctx) const override;
    std::string_view name() const override;

private:
    double a_;
    double b_;
};

// Model 3.8: dy/dt = -a1*y - a2*y^2 + b*u (Euler method)
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
