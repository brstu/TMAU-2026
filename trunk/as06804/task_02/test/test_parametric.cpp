#include <gtest/gtest.h>

#include <cmath>
#include <memory>
#include <sstream>
#include <utility>
#include <vector>

#include "DiffEqModel.h"
#include "LinearModel.h"
#include "NonLinearModel.h"
#include "InputSignal.h"
#include "SimulationRunner.h"

namespace {

class VectorSignal final : public Signal {
public:
    explicit VectorSignal(std::vector<double> values) : values_(std::move(values)) {}

    double value(int step, double) const override {
        return values_[static_cast<std::size_t>(step)];
    }

private:
    std::vector<double> values_;
};

enum class ModelKind { Linear, NonLinear, DiffEq };

struct StepCase {
    ModelKind kind;
    double p1;
    double p2;
    double p3;
    double y0;
    double dt;
    std::vector<double> inputs;
    std::vector<double> expected;
};

std::unique_ptr<Model> createModel(const StepCase& c) {
    switch (c.kind) {
        case ModelKind::Linear:
            return std::make_unique<LinearModel>(c.p1, c.p2, c.y0, c.dt);
        case ModelKind::NonLinear:
            return std::make_unique<NonLinearModel>(c.p1, c.p2, c.p3, c.y0, c.dt);
        case ModelKind::DiffEq:
        default:
            return std::make_unique<DiffEqModel>(c.p1, c.p2, c.p3, c.y0, c.dt);
    }
}

class StepParametricTest : public testing::TestWithParam<StepCase> {};

TEST_P(StepParametricTest, SimulationStepsMatchHandCalculatedValues) {
    const StepCase& c = GetParam();
    auto model = createModel(c);
    VectorSignal signal(c.inputs);
    std::ostringstream sink;

    const SimulationRecord record =
        runSimulation(*model, signal, static_cast<int>(c.expected.size()), c.dt, sink);

    ASSERT_EQ(record.output.size(), c.expected.size());
    for (std::size_t i = 0; i < c.expected.size(); ++i) {
        EXPECT_NEAR(record.output[i], c.expected[i], 1e-9) << "sample " << i;
    }
    EXPECT_FALSE(record.diverged);
}

INSTANTIATE_TEST_SUITE_P(
    AllModelKinds,
    StepParametricTest,
    testing::Values(
        StepCase{ModelKind::Linear, 0.5, 2.0, 0.0, 1.0, 0.1,
                 {0.0, 1.0, 1.0}, {0.5, 2.25, 1.125}},
        StepCase{ModelKind::Linear, 0.0, 1.0, 0.0, 0.0, 0.1,
                 {1.0, 0.0, 0.0}, {0.0, -1.0, 0.0}},
        StepCase{ModelKind::NonLinear, 0.5, 0.2, 1.0, 1.0, 0.1,
                 {0.0, 0.0, 2.0}, {0.3, -0.1865, 3.9367565551875}},
        StepCase{ModelKind::DiffEq, 1.0, 0.0, 2.0, 1.0, 0.1,
                 {1.0, 1.0, 0.0}, {1.1, 1.19, 1.071}}
    )
);

// ---------------------------------------------------------------------------
// Convergence of the discrete linear model for different |a|.
// ---------------------------------------------------------------------------

struct ConvergenceCase {
    double a;
    double y0;
    int steps;
    double expected;
    double tolerance;
};

class ConvergenceParametricTest : public testing::TestWithParam<ConvergenceCase> {};

TEST_P(ConvergenceParametricTest, FreeProcessBehaviorMatchesExpectation) {
    const auto c = GetParam();
    LinearModel model(c.a, 1.0, c.y0, 0.1);
    VectorSignal signal(std::vector<double>(static_cast<std::size_t>(c.steps), 0.0));
    std::ostringstream sink;

    const SimulationRecord record = runSimulation(model, signal, c.steps, 0.1, sink);
    ASSERT_FALSE(record.output.empty());
    EXPECT_NEAR(record.output.back(), c.expected, c.tolerance);
}

INSTANTIATE_TEST_SUITE_P(
    ConvergenceScenarios,
    ConvergenceParametricTest,
    testing::Values(
        ConvergenceCase{0.5, 1.0, 20, 0.0, 1e-5},
        ConvergenceCase{-0.5, 1.0, 20, 0.0, 1e-5},
        ConvergenceCase{0.99, 1.0, 20, 0.8179, 1e-3},
        ConvergenceCase{1.0, 1.0, 20, 1.0, 1e-12},
        ConvergenceCase{1.01, 1.0, 20, 1.22019, 1e-3}
    )
);

// ---------------------------------------------------------------------------
// Explicit Euler vs. analytical solution for several discretization steps.
// ---------------------------------------------------------------------------

struct EulerCase {
    double a1;
    double dt;
};

class EulerParametricTest : public testing::TestWithParam<EulerCase> {};

TEST_P(EulerParametricTest, EulerStepIsCloseToAnalyticalSolution) {
    const auto c = GetParam();
    const int steps = static_cast<int>(std::lround(1.0 / c.dt));

    DiffEqModel model(c.a1, 0.0, 1.0, 0.0, c.dt);
    StepSignal signal;
    std::ostringstream sink;
    const SimulationRecord record = runSimulation(model, signal, steps, c.dt, sink);

    const double exact = (1.0 / c.a1) * (1.0 - std::exp(-c.a1));
    ASSERT_FALSE(record.output.empty());
    EXPECT_NEAR(record.output.back(), exact, 0.2 * c.dt + 1e-6);
}

INSTANTIATE_TEST_SUITE_P(
    EulerStepSizes,
    EulerParametricTest,
    testing::Values(
        EulerCase{0.5, 0.1}, EulerCase{0.5, 0.05}, EulerCase{0.5, 0.01},
        EulerCase{1.0, 0.1}, EulerCase{1.0, 0.05}, EulerCase{1.0, 0.01},
        EulerCase{2.0, 0.1}, EulerCase{2.0, 0.05}, EulerCase{2.0, 0.01}
    )
);

// ---------------------------------------------------------------------------
// Substitution of input signals through the Signal interface.
// ---------------------------------------------------------------------------

struct SignalModeCase {
    int type;
    std::vector<double> expected;
};

class SignalModeParametricTest : public testing::TestWithParam<SignalModeCase> {};

TEST_P(SignalModeParametricTest, FactorySignalFeedsSimulation) {
    const auto c = GetParam();
    auto signal = makeSignal(c.type);
    LinearModel model(0.5, 1.0, 0.0, 0.1);
    std::ostringstream sink;

    const SimulationRecord record =
        runSimulation(model, *signal, static_cast<int>(c.expected.size()), 0.1, sink);

    ASSERT_EQ(record.input.size(), c.expected.size());
    for (std::size_t i = 0; i < c.expected.size(); ++i) {
        EXPECT_NEAR(record.input[i], c.expected[i], 1e-12) << "sample " << i;
    }
}

INSTANTIATE_TEST_SUITE_P(
    InputModes,
    SignalModeParametricTest,
    testing::Values(
        SignalModeCase{1, {1.0, 1.0, 1.0}},
        SignalModeCase{2, {1.0, 0.0, 0.0}},
        SignalModeCase{3, {0.0, std::sin(0.1), std::sin(0.2)}}
    )
);

} // namespace
