#include "StabilityAnalyzer.h"

#include <gtest/gtest.h>

#include <cmath>
#include <limits>
#include <memory>
#include <stdexcept>

namespace task02 {
namespace {

struct DelayedCase {
    const char* name;
    double a;
    double b;
    int delay;
    std::vector<double> inputs;
    std::vector<double> outputs;
};

class DelayedSimulation : public ::testing::TestWithParam<DelayedCase> {};

TEST_P(DelayedSimulation, MatchesReferenceTrajectory) {
    const auto& test = GetParam();
    const DelayedControl model(test.a, test.b, test.delay);
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

INSTANTIATE_TEST_SUITE_P(Variant21, DelayedSimulation, ::testing::Values(
    DelayedCase{"Step", 0.5, 2.0, 2, {1, 1, 1, 1, 1, 1}, {0, 0, 0, 2, 3, 3.5}},
    DelayedCase{"Impulse", 0.5, 2.0, 2, {1, 0, 0, 0, 0, 0}, {0, 0, 0, 2, 1, 0.5}},
    DelayedCase{"Harmonic", 0.5, 2.0, 1, {0, 1, 0, -1, 0, 1}, {0, 0, 0, 2, 1, -1.5}},
    DelayedCase{"NoDelay", -0.5, 2.0, 0, {1, 1, 1, 1}, {1, 1.5, 1.25, 1.375}},
    DelayedCase{"DelayBeyondEnd", 0.5, 2.0, 5, {1, 1, 1}, {4, 2, 1}},
    DelayedCase{"AtRest", 0.7, 8.0, 1, {0, 0, 0}, {0, 0, 0}},
    DelayedCase{"UnstableFreeResponse", -2.0, 3.0, 1, {0, 0, 0, 0}, {1, -2, 4, -8}}
), [](const auto& info) { return info.param.name; });

TEST(DelayedControlTest, StableStepResponseApproachesEquilibrium) {
    const DelayedControl model(0.5, 2.0, 2);
    const auto samples = model.simulate(50, 0.0, 1.0, makeInputSignal(InputType::Step));
    EXPECT_NEAR(samples.back().output, 4.0, 1e-12);
}

TEST(DelayedControlTest, RejectsNegativeDelay) {
    EXPECT_THROW(DelayedControl(0.5, 2.0, -1), std::invalid_argument);
}

struct RelayCase {
    const char* name;
    double state;
    double input;
    double expected;
};

class RelayStep : public ::testing::TestWithParam<RelayCase> {};

TEST_P(RelayStep, SelectsCorrectBranch) {
    const HysteresisRelay model(0.5, 3.0, 0.25);
    const auto& test = GetParam();
    EXPECT_DOUBLE_EQ(model.next(test.state, test.input, 1.0), test.expected);
}

INSTANTIATE_TEST_SUITE_P(Variant21, RelayStep, ::testing::Values(
    RelayCase{"AboveUpper", -2.0, 0.5, 2.0},
    RelayCase{"BelowLower", 2.0, -0.5, -2.0},
    RelayCase{"AtUpperPositiveState", 2.0, 0.25, 4.0},
    RelayCase{"AtUpperNegativeState", -2.0, 0.25, -4.0},
    RelayCase{"AtLowerPositiveState", 2.0, -0.25, 4.0},
    RelayCase{"AtLowerNegativeState", -2.0, -0.25, -4.0},
    RelayCase{"InBandPositiveState", 2.0, 0.0, 4.0},
    RelayCase{"InBandNegativeState", -2.0, 0.0, -4.0},
    RelayCase{"ZeroInputAndZeroState", 0.0, 0.0, -3.0}
), [](const auto& info) { return info.param.name; });

TEST(HysteresisRelayTest, KeepsBranchUntilInputCrossesOppositeThreshold) {
    const HysteresisRelay model(0.5, 2.0, 0.25);
    const std::vector<double> inputs{1.0, 0.25, -0.25, -1.0, 0.25, 0.0};
    const std::vector<double> expected{0.0, 2.0, 3.0, 3.5, -0.25, -2.125};
    const auto samples = model.simulate(5, 0.0, 1.0,
        [&](int step) { return inputs.at(step); });
    ASSERT_EQ(samples.size(), expected.size());
    for (std::size_t step = 0; step < samples.size(); ++step) {
        EXPECT_DOUBLE_EQ(samples[step].output, expected[step]);
    }
}

TEST(HysteresisRelayTest, RejectsInvalidThreshold) {
    EXPECT_THROW(HysteresisRelay(0.5, 3.0, -0.25), std::invalid_argument);
    EXPECT_THROW(HysteresisRelay(0.5, 3.0, std::numeric_limits<double>::infinity()),
                 std::invalid_argument);
}

struct EulerCase {
    double b;
    double input;
    double dt;
    double initialState;
    double expected;
};

class EulerStep : public ::testing::TestWithParam<EulerCase> {};

TEST_P(EulerStep, MatchesAnalyticalSolutionForConstantInput) {
    const auto& test = GetParam();
    const HarmonicForce model(test.b);
    const auto samples = model.simulate(4, test.initialState, test.dt,
        [&](int) { return test.input; });
    ASSERT_EQ(samples.size(), 5u);
    EXPECT_NEAR(samples.back().output, test.expected, 1e-12);
}

INSTANTIATE_TEST_SUITE_P(Variant21, EulerStep, ::testing::Values(
    EulerCase{2.0, std::asin(1.0), 0.25, 2.0, 4.0},
    EulerCase{2.0, -std::asin(1.0), 0.125, 1.0, 0.0},
    EulerCase{3.0, 0.0, 0.25, 0.0, 0.0},
    EulerCase{-2.0, std::asin(0.5), 0.5, 0.0, -2.0}
));

class EulerRefinement : public ::testing::TestWithParam<int> {};

TEST_P(EulerRefinement, HalvingStepHalvesErrorForLinearDerivative) {
    const HarmonicForce model(2.0);
    const auto error = [&](int steps) {
        const double dt = 1.0 / steps;
        const auto values = model.simulate(steps, 0.0, dt,
            [dt](int step) { return std::asin(step * dt); });
        // u(t) = asin(t) yields dy/dt = 2*t and y(1) = 1.
        return 1.0 - values.back().output;
    };
    const int steps = GetParam();
    EXPECT_NEAR(error(steps), 1.0 / steps, 1e-12);
    EXPECT_NEAR(error(steps * 2), error(steps) / 2.0, 1e-12);
}

INSTANTIATE_TEST_SUITE_P(Variant21, EulerRefinement, ::testing::Values(4, 8, 16));

TEST(SimulationTest, RejectsInvalidParametersAndNonFiniteValues) {
    const HarmonicForce model(1.0);
    const auto zero = [](int) { return 0.0; };
    const double inf = std::numeric_limits<double>::infinity();
    EXPECT_THROW(model.simulate(-1, 0.0, 1.0, zero), std::invalid_argument);
    EXPECT_THROW(model.simulate(2, inf, 1.0, zero), std::invalid_argument);
    EXPECT_THROW(model.simulate(2, 0.0, inf, zero), std::invalid_argument);
    EXPECT_THROW(model.simulate(2, 0.0, 0.0, zero), std::invalid_argument);
    EXPECT_THROW(model.simulate(2, 0.0, 1.0, [inf](int) { return inf; }), std::runtime_error);

    const DelayedControl diverging(2.0, 0.0, 0);
    EXPECT_THROW(diverging.simulate(1, std::numeric_limits<double>::max(), 1.0, zero),
                 std::runtime_error);
}

TEST(SimulationTest, IncludesInitialSampleEvenWithZeroSteps) {
    const HarmonicForce model(1.0);
    const auto samples = model.simulate(0, 3.5, 0.25, makeInputSignal(InputType::Impulse));
    ASSERT_EQ(samples.size(), 1u);
    EXPECT_EQ(samples[0].step, 0);
    EXPECT_DOUBLE_EQ(samples[0].input, 1.0);
    EXPECT_DOUBLE_EQ(samples[0].output, 3.5);
}

TEST(ModelTest, NamesIdentifyTheSelectedVariantModels) {
    const std::unique_ptr<Model> delayed = std::make_unique<DelayedControl>(0.5, 1.0, 1);
    const std::unique_ptr<Model> relay = std::make_unique<HysteresisRelay>(0.5, 1.0, 0.5);
    const std::unique_ptr<Model> harmonic = std::make_unique<HarmonicForce>(1.0);
    EXPECT_EQ(delayed->name().substr(0, 9), "Model 1.2");
    EXPECT_EQ(relay->name().substr(0, 9), "Model 2.7");
    EXPECT_EQ(harmonic->name().substr(0, 9), "Model 3.5");
}

} // namespace
} // namespace task02
