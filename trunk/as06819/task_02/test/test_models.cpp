#include "StabilityAnalyzer.h"

#include <gtest/gtest.h>

#include <cmath>
#include <limits>
#include <memory>
#include <stdexcept>
#include <vector>

namespace task02 {
namespace {

// --- Тестирование Model 1.10 (Distributed Control Window Model) ---

struct WindowCase {
    const char* name;
    double a1;
    double b1;
    double b2;
    std::vector<double> inputs;
    std::vector<double> outputs;
};

class DistributedWindowSimulation : public ::testing::TestWithParam<WindowCase> {};

TEST_P(DistributedWindowSimulation, MatchesReferenceTrajectory) {
    const auto& test = GetParam();
    const DistributedControlWindow model(test.a1, test.b1, test.b2);
    const auto samples = model.simulate(static_cast<int>(test.outputs.size()) - 1,
        test.outputs.front(), 1.0, [&](int step) { return test.inputs.at(step); });
    ASSERT_EQ(samples.size(), test.outputs.size());
    for (std::size_t step = 0; step < samples.size(); ++step) {
        SCOPED_TRACE(step);
        EXPECT_EQ(samples[step].step, static_cast<int>(step));
        EXPECT_DOUBLE_EQ(samples[step].input, test.inputs[step]);
        EXPECT_NEAR(samples[step].output, test.outputs[step], 1e-12);
    }
}

INSTANTIATE_TEST_SUITE_P(Variant20, DistributedWindowSimulation, ::testing::Values(
    WindowCase{"Step", 0.5, 2.0, 1.0, {1, 1, 1, 1, 1}, {0, 2, 3, 4.5, 5.25}},
    WindowCase{"Impulse", 0.5, 2.0, 1.0, {1, 0, 0, 0, 0}, {0, 2, 1, 1.5, 0.75}},
    WindowCase{"AtRest", 0.7, 3.0, -1.5, {0, 0, 0, 0}, {0, 0, 0, 0}},
    WindowCase{"UnstableFreeResponse", -2.0, 1.0, 1.0, {0, 0, 0, 0}, {1, -2, 4, -8}}
), [](const auto& info) { return info.param.name; });

TEST(DistributedControlWindowTest, StableStepResponseApproachesEquilibrium) {
    const DistributedControlWindow model(0.5, 2.0, 1.0);
    const auto samples = model.simulate(50, 0.0, 1.0, makeInputSignal(InputType::Step));
    EXPECT_NEAR(samples.back().output, 6.0, 1e-12);
}

TEST(DistributedControlWindowTest, RejectsNonFiniteCoefficients) {
    const double inf = std::numeric_limits<double>::infinity();
    EXPECT_THROW(DistributedControlWindow(inf, 1.0, 1.0), std::invalid_argument);
    EXPECT_THROW(DistributedControlWindow(0.5, inf, 1.0), std::invalid_argument);
    EXPECT_THROW(DistributedControlWindow(0.5, 1.0, inf), std::invalid_argument);
}

// --- Тестирование Model 2.4 (Logarithmic Multi-Variable Interaction) ---

struct LogCase {
    const char* name;
    double a;
    double b;
    double y;
    double u;
    double expected;
};

class LogarithmicStep : public ::testing::TestWithParam<LogCase> {};

TEST_P(LogarithmicStep, ComputesSingleStepAccurately) {
    const auto& test = GetParam();
    const LogarithmicInteraction model(test.a, test.b);
    EXPECT_NEAR(model.next({test.y, test.u, 0.0, 1.0}), test.expected, 1e-12);
}

INSTANTIATE_TEST_SUITE_P(Variant20, LogarithmicStep, ::testing::Values(
    LogCase{"AtRestZeroInputZeroState", 0.8, 2.5, 0.0, 0.0, 0.0},
    LogCase{"PositiveInputZeroState", 0.5, 2.0, 0.0, std::exp(1.0) - 1.0, 2.0},
    LogCase{"NegativeInputSymmetry", 0.5, 2.0, 0.0, -(std::exp(1.0) - 1.0), 2.0},
    LogCase{"NonZeroStateWithCosine", 2.0, 3.0, 0.0, 0.0, 0.0},
    LogCase{"PiHalfStateZeroesCosineTerm", 5.0, 1.0, std::acos(0.0), std::exp(2.0) - 1.0, 2.0}
), [](const auto& info) { return info.param.name; });

TEST(LogarithmicInteractionTest, RejectsNonFiniteCoefficients) {
    const double nanVal = std::numeric_limits<double>::quiet_NaN();
    EXPECT_THROW(LogarithmicInteraction(nanVal, 1.0), std::invalid_argument);
    EXPECT_THROW(LogarithmicInteraction(1.0, nanVal), std::invalid_argument);
}

// --- Тестирование Model 3.8 (Combined Linear-Quadratic Decay, Euler) ---

struct EulerBernoulliCase {
    double a1;
    double a2;
    double b;
    double y0;
    double u;
    double dt;
    double expected;
};

class EulerDecayStep : public ::testing::TestWithParam<EulerBernoulliCase> {};

TEST_P(EulerDecayStep, ComputesSingleEulerStepAccurately) {
    const auto& test = GetParam();
    const CombinedQuadraticDecay model(test.a1, test.a2, test.b);
    EXPECT_NEAR(model.next({test.y0, test.u, 0.0, test.dt}), test.expected, 1e-12);
}

INSTANTIATE_TEST_SUITE_P(Variant20, EulerDecayStep, ::testing::Values(
    EulerBernoulliCase{1.0, 2.0, 3.0, 0.0, 0.0, 0.1, 0.0},
    EulerBernoulliCase{1.0, 0.5, 4.0, 2.0, 1.0, 0.25, 2.0},
    EulerBernoulliCase{2.0, 1.0, 0.0, 1.0, 0.0, 0.1, 0.7}
));

class EulerConvergence : public ::testing::TestWithParam<int> {};

TEST_P(EulerConvergence, HalvingStepHalvesGlobalErrorAgainstAnalyticalBernoulliSolution) {
    const double a1 = 1.0;
    const double a2 = 1.0;
    const double y0 = 1.0;
    const double exactY1 = 1.0 / (2.0 * std::exp(1.0) - 1.0);
    const CombinedQuadraticDecay model(a1, a2, 0.0);

    const auto globalError = [&](int steps) {
        const double dt = 1.0 / steps;
        const auto samples = model.simulate(steps, y0, dt, [](int) { return 0.0; });
        return std::abs(exactY1 - samples.back().output);
    };

    const int steps = GetParam();
    const double err1 = globalError(steps);
    const double err2 = globalError(steps * 2);
    EXPECT_NEAR(err1 / err2, 2.0, 0.1);
}

INSTANTIATE_TEST_SUITE_P(Variant20, EulerConvergence, ::testing::Values(32, 64, 128));

TEST(CombinedQuadraticDecayTest, RejectsNonFiniteCoefficients) {
    const double inf = std::numeric_limits<double>::infinity();
    EXPECT_THROW(CombinedQuadraticDecay(inf, 1.0, 1.0), std::invalid_argument);
    EXPECT_THROW(CombinedQuadraticDecay(1.0, inf, 1.0), std::invalid_argument);
    EXPECT_THROW(CombinedQuadraticDecay(1.0, 1.0, inf), std::invalid_argument);
}

// --- Общие тесты симуляции и полиморфизма ---

TEST(SimulationTest, RejectsInvalidParametersAndNonFiniteValues) {
    const CombinedQuadraticDecay model(1.0, 0.5, 1.0);
    const auto zero = [](int) { return 0.0; };
    const double inf = std::numeric_limits<double>::infinity();
    EXPECT_THROW(model.simulate(-1, 0.0, 1.0, zero), std::invalid_argument);
    EXPECT_THROW(model.simulate(2, inf, 1.0, zero), std::invalid_argument);
    EXPECT_THROW(model.simulate(2, 0.0, inf, zero), std::invalid_argument);
    EXPECT_THROW(model.simulate(2, 0.0, 0.0, zero), std::invalid_argument);
    EXPECT_THROW(model.simulate(2, 0.0, 1.0, [inf](int) { return inf; }), std::runtime_error);

    const DistributedControlWindow diverging(2.0, 0.0, 0.0);
    EXPECT_THROW(diverging.simulate(1, std::numeric_limits<double>::max(), 1.0, zero),
                 std::runtime_error);
}

TEST(SimulationTest, IncludesInitialSampleEvenWithZeroSteps) {
    const CombinedQuadraticDecay model(1.0, 1.0, 1.0);
    const auto samples = model.simulate(0, 3.5, 0.25, makeInputSignal(InputType::Impulse));
    ASSERT_EQ(samples.size(), 1u);
    EXPECT_EQ(samples[0].step, 0);
    EXPECT_DOUBLE_EQ(samples[0].input, 1.0);
    EXPECT_DOUBLE_EQ(samples[0].output, 3.5);
}


TEST(ModelTest, NamesIdentifyTheSelectedVariant20Models) {
    const std::unique_ptr<Model> linear = std::make_unique<DistributedControlWindow>(0.5, 1.0, 0.5);
    const std::unique_ptr<Model> nonlinear = std::make_unique<LogarithmicInteraction>(0.5, 1.0);
    const std::unique_ptr<Model> diff = std::make_unique<CombinedQuadraticDecay>(1.0, 0.5, 1.0);
    EXPECT_EQ(linear->name().substr(0, 10), "Model 1.10");
    EXPECT_EQ(nonlinear->name().substr(0, 9), "Model 2.4");
    EXPECT_EQ(diff->name().substr(0, 9), "Model 3.8");
}

} // namespace
} // namespace task02
