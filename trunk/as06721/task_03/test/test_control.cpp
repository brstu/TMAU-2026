#include "control.hpp"

#include <gtest/gtest.h>

#include <array>
#include <cmath>
#include <limits>
#include <stdexcept>

namespace lab03 {
namespace {

const PidSettings proportional{1, 0, 0, -100, 100};
const PlantSettings direct{PlantKind::delayedLinear, 1, 1};

TEST(PidTest, AppliesIncrementalPidFormula) {
    Pid pid({2, 0.5, 0.25, -100, 100}, 0.5);
    EXPECT_DOUBLE_EQ(pid.advance(2), 5.5);
    EXPECT_DOUBLE_EQ(pid.advance(1), 2.25);
    EXPECT_DOUBLE_EQ(pid.advance(-1), -2.5);
}

TEST(PidTest, RecoversFromSaturationWithoutStoringExcessIntegral) {
    Pid pid({0, 1, 0, -1, 1}, 1);
    for (int step = 0; step < 10; ++step) {
        EXPECT_DOUBLE_EQ(pid.advance(2), 1);
    }
    EXPECT_DOUBLE_EQ(pid.advance(-0.5), 0.5);
    EXPECT_DOUBLE_EQ(pid.advance(-2), -1);
    EXPECT_DOUBLE_EQ(pid.advance(0.75), -0.25);
}

TEST(PidTest, RejectsInvalidGainsPeriodAndActuatorLimits) {
    EXPECT_THROW(Pid(proportional, 0), std::invalid_argument);
    EXPECT_THROW(Pid((PidSettings{-1, 0, 0, -5, 5}), 1), std::invalid_argument);
    EXPECT_THROW(Pid((PidSettings{1, 0, 0, 1, 5}), 1), std::invalid_argument);
    EXPECT_THROW(Pid((PidSettings{1, 0, 0, 0, 0}), 1), std::invalid_argument);
    Pid pid(proportional, 1);
    EXPECT_THROW(pid.advance(std::numeric_limits<double>::infinity()), std::invalid_argument);
}

TEST(SimulationTest, PureInputDelayUsesZeroPrehistoryAndResetsForEachRun) {
    const PlantSettings plant{PlantKind::delayedLinear, 0.5, 2, 2};
    const SimulationSettings settings{StopMode::fixedSteps, 5, 0.25, 0, 1};
    const std::array<double, 6> expected{0, 0, 0, 2, 3, 3.5};
    for (int run = 0; run < 2; ++run) {
        const auto result = simulate(plant, proportional, settings);
        ASSERT_EQ(result.samples.size(), expected.size());
        for (std::size_t k = 0; k < expected.size(); ++k) {
            SCOPED_TRACE(k);
            const auto& row = result.samples[k];
            EXPECT_EQ(row.step, k);
            EXPECT_DOUBLE_EQ(row.time, 0.25 * k);
            EXPECT_DOUBLE_EQ(row.output, expected[k]);
            EXPECT_DOUBLE_EQ(row.error, 1.0 - expected[k]);
        }
        EXPECT_DOUBLE_EQ(result.samples[0].command, 0);
        EXPECT_DOUBLE_EQ(result.samples[1].command, 1);
        EXPECT_DOUBLE_EQ(result.samples[4].command, -1);
    }
}

struct PlantStep {
    const char* name;
    PlantSettings plant;
    double initial;
    double target;
    double dt;
    double expected;
};

class PlantStepTest : public ::testing::TestWithParam<PlantStep> {};

TEST_P(PlantStepTest, MatchesTheReferenceEquation) {
    const auto& test = GetParam();
    const auto result = simulate(test.plant, proportional,
        {StopMode::fixedSteps, 1, test.dt, test.initial, test.target});
    ASSERT_EQ(result.samples.size(), 2u);
    EXPECT_NEAR(result.samples.back().output, test.expected, 1e-12);
}

const PlantSettings linear{PlantKind::delayedLinear, 0.5, 2};
const PlantSettings relay{PlantKind::relay, 0.5, 3, 0, 0.25};
const PlantSettings harmonic{PlantKind::harmonic, 0, 2};

INSTANTIATE_TEST_SUITE_P(Variant21, PlantStepTest, ::testing::Values(
    PlantStep{"ImmediateLinearInput", linear, 1, 2, 0.25, 2.5},
    PlantStep{"PositiveRelayAtLowerThreshold", relay, 2, 1.75, 1, 4},
    PlantStep{"NegativeRelayAtUpperThreshold", relay, -2, -1.75, 1, -4},
    PlantStep{"RelaySwitchesPositive", relay, -2, -1.5, 1, 2},
    PlantStep{"RelaySwitchesNegative", relay, 2, 1.5, 1, -2},
    PlantStep{"RelayAtZero", relay, 0, 0, 1, -3},
    PlantStep{"PositiveEulerStep", harmonic, 1, 1 + std::asin(1.0), 0.25, 1.5},
    PlantStep{"NegativeEulerStep", harmonic, 1, 1 - std::asin(1.0), 0.25, 0.5},
    PlantStep{"ZeroEulerStep", harmonic, 0, 0, 0.25, 0}
), [](const auto& info) { return info.param.name; });

TEST(SimulationTest, FixedModeCompletesAllStepsEvenAtTarget) {
    const auto result = simulate(direct, proportional, {StopMode::fixedSteps, 8, 1, 0, 1});
    EXPECT_EQ(result.reason, StopReason::completed);
    ASSERT_EQ(result.samples.size(), 9u);
    EXPECT_DOUBLE_EQ(result.samples.back().output, 1);
}

TEST(SimulationTest, ErrorModeStopsImmediatelyIfInitialSampleIsInsideBand) {
    const auto result = simulate(direct, proportional,
        {StopMode::errorTolerance, 10, 1, 1, 1.25, 0.25});
    EXPECT_EQ(result.reason, StopReason::toleranceReached);
    ASSERT_EQ(result.samples.size(), 1u);
    EXPECT_DOUBLE_EQ(result.samples.front().command, 0);
}

TEST(SimulationTest, ErrorModeAcceptsTargetReachedOnLastAllowedStep) {
    const auto result = simulate(direct, proportional,
        {StopMode::errorTolerance, 1, 1, 0, 1, 0});
    EXPECT_EQ(result.reason, StopReason::toleranceReached);
    ASSERT_EQ(result.samples.size(), 2u);
    EXPECT_DOUBLE_EQ(result.samples.back().error, 0);
}

TEST(SimulationTest, ErrorModeStopsAtLimitIfTargetIsUnreachable) {
    const PlantSettings bounded{PlantKind::delayedLinear, 0, 1};
    const PidSettings limited{0, 1, 0, -0.5, 0.5};
    const auto result = simulate(bounded, limited, {StopMode::errorTolerance, 2, 1, 0, 4, 0.1});
    EXPECT_EQ(result.reason, StopReason::stepLimit);
    ASSERT_EQ(result.samples.size(), 3u);
    EXPECT_DOUBLE_EQ(result.samples.back().error, 3.5);
}

TEST(SimulationTest, RejectsInvalidSamplePeriodAndNegativeTolerance) {
    EXPECT_THROW(simulate(direct, proportional, {StopMode::fixedSteps, 1, 0, 0, 0}),
        std::invalid_argument);
    EXPECT_THROW(simulate(direct, proportional, {StopMode::errorTolerance, 1, 1, 0, 0, -1}),
        std::invalid_argument);
    EXPECT_THROW(simulate({PlantKind::relay, 0, 1, 0, -1}, proportional,
        {StopMode::fixedSteps, 1, 1, 0, 0}), std::invalid_argument);
}

} // namespace
} // namespace lab03
