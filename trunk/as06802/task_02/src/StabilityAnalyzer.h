#pragma once

#include <functional>
#include <iosfwd>
#include <string_view>
#include <vector>

namespace var17 {

struct TrajectoryPoint {
    int tau;
    double control;
    double state;
};

struct StepInputs {
    double yCurr;
    double yPrev;
    double uCurr;
    double uPrev1;
    double uPrev2;
    double stepSize;
};

enum class SignalKind { Step = 1, Impulse = 2, Harmonic = 3 };
using ControlGenerator = std::function<double(int)>;
ControlGenerator buildControlGenerator(SignalKind kind);

class BaseSystemModel {
public:
    virtual ~BaseSystemModel();
    virtual double evaluateNext(const StepInputs& in) const = 0;
    virtual std::string_view title() const = 0;

    std::vector<TrajectoryPoint> runSimulation(int totalSteps, double initialState,
                                               double dt, const ControlGenerator& gen) const;
};

/// Multi-step control history linear discrete system (Variant 17, Model 1.7).
class MultiStepControlSystem final : public BaseSystemModel {
public:
    MultiStepControlSystem(double a, double b1, double b2, double b3);
    double evaluateNext(const StepInputs& in) const override;
    std::string_view title() const override;

private:
    double a_;
    double b1_;
    double b2_;
    double b3_;
};

/// Quadratic feedback with harmonic control nonlinear system (Variant 17, Model 2.1).
class QuadraticHarmonicSystem final : public BaseSystemModel {
public:
    QuadraticHarmonicSystem(double a, double b, double c, double d);
    double evaluateNext(const StepInputs& in) const override;
    std::string_view title() const override;

private:
    double a_;
    double b_;
    double c_;
    double d_;
};

/// Harmonic driving force differential equation via Euler scheme (Variant 17, Model 3.5).
class HarmonicDriveEulerSystem final : public BaseSystemModel {
public:
    explicit HarmonicDriveEulerSystem(double b);
    double evaluateNext(const StepInputs& in) const override;
    std::string_view title() const override;

private:
    double b_;
};

enum class PoleMode { AsymptoticallyStable, BoundaryMarginal, DivergentUnstable };

struct PoleReport {
    double poleLocation;
    PoleMode mode;

    bool isConvergent() const;
};

PoleReport inspectLinearStability(double a);
void printStabilityAlert(std::ostream& out, const PoleReport& report);

} // namespace var17
