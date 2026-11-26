#pragma once

#include <cmath>
#include <functional>
#include <iosfwd>
#include <stdexcept>
#include <string_view>
#include <vector>

namespace var16 {

class ModelExecutionError final : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

struct SimulationPoint {
    int step;
    double uVal;
    double yVal;
};

struct StepFrame {
    double y0;
    double y1;
    double y2;
    double u0;
    double dt;
};

enum class InputMode { Step = 1, Impulse = 2, Harmonic = 3 };
using InputFn = std::function<double(int)>;
InputFn createInputFn(InputMode mode);

class AbstractDynamicModel {
public:
    virtual ~AbstractDynamicModel();
    virtual double computeStep(const StepFrame& frame) const = 0;
    virtual std::string_view label() const = 0;

    template <typename SignalCallable>
    std::vector<SimulationPoint> execute(int count, double startY,
                                         double dt, SignalCallable&& signal) const {
        if (count < 0 || !std::isfinite(startY) || !std::isfinite(dt) || dt <= 0.0) {
            throw std::invalid_argument("Недопустимые параметры запуска моделирования.");
        }

        std::vector<SimulationPoint> trace;
        trace.reserve(static_cast<std::size_t>(count) + 1);
        double yCurr = startY;

        for (int idx = 0; ; ++idx) {
            const double uCurr = signal(idx);
            if (!std::isfinite(uCurr)) {
                throw ModelExecutionError("Входной сигнал вернул нечисловое или бесконечное значение.");
            }
            trace.push_back({idx, uCurr, yCurr});
            if (idx == count) {
                break;
            }

            const double yPrev1 = (idx >= 1) ? trace[static_cast<std::size_t>(idx - 1)].yVal : 0.0;
            const double yPrev2 = (idx >= 2) ? trace[static_cast<std::size_t>(idx - 2)].yVal : 0.0;

            yCurr = computeStep({yCurr, yPrev1, yPrev2, uCurr, dt});
            if (!std::isfinite(yCurr)) {
                throw ModelExecutionError("Состояние системы вышло за пределы конечных чисел.");
            }
        }
        return trace;
    }
};

/// Third-order linear dynamic system (Variant 16, Model 1.6).
class ThirdOrderLinearModel final : public AbstractDynamicModel {
public:
    ThirdOrderLinearModel(double a1, double a2, double a3, double b);
    double computeStep(const StepFrame& frame) const override;
    std::string_view label() const override;

private:
    double a1_;
    double a2_;
    double a3_;
    double b_;
};

/// Hyperbolic tangent smoothing nonlinear system (Variant 16, Model 2.10).
class TanhCubicNonlinearModel final : public AbstractDynamicModel {
public:
    TanhCubicNonlinearModel(double a, double b);
    double computeStep(const StepFrame& frame) const override;
    std::string_view label() const override;

private:
    double a_;
    double b_;
};

/// Quadratic self-decay differential equation solved via Euler method (Variant 16, Model 3.4).
class QuadraticSelfDecayModel final : public AbstractDynamicModel {
public:
    explicit QuadraticSelfDecayModel(double a);
    double computeStep(const StepFrame& frame) const override;
    std::string_view label() const override;

private:
    double a_;
};

enum class StabilityZone { AsymptoticallyStable, CriticalBoundary, UnstableDivergent };

struct JuryReport {
    StabilityZone zone;
    double pAtOne;
    double negPAtMinusOne;
    double absA3;
    double juryMargin;

    bool isStable() const;
};

JuryReport evaluateThirdOrderStability(double a1, double a2, double a3);
void reportStabilityStatus(std::ostream& stream, const JuryReport& report);

} // namespace var16
