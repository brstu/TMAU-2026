#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cmath>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "InputSignal.h"
#include "MockSignalSource.h"
#include "Model17.h"
#include "Model21.h"
#include "Model35.h"
#include "Simulator.h"

using ::testing::InSequence;
using ::testing::Return;

namespace {

struct SimulatorModeCase {
    SignalType type;
    std::vector<double> expectedY;
};

} // namespace

TEST(SimulatorTest, ThrowsOnNonPositiveSteps) {
    Model17 model(0.7, 0.5, 0.2, 0.1);
    InputSignal signal(SignalType::Step);

    EXPECT_THROW(Simulator(model, signal, 0), std::invalid_argument);
    EXPECT_THROW(Simulator(model, signal, -5), std::invalid_argument);
}

TEST(SimulatorTest, UsesMockSignalSourceToDriveModel) {
    Model17 model(0.0, 1.0, 0.0, 0.0);
    MockSignalSource mock;

    {
        InSequence sequence;
        EXPECT_CALL(mock, value(0)).WillOnce(Return(1.0));
        EXPECT_CALL(mock, value(1)).WillOnce(Return(2.0));
        EXPECT_CALL(mock, value(2)).WillOnce(Return(3.0));
    }

    Simulator simulator(model, mock, 3);
    const SimulationResult result = simulator.run();

    ASSERT_EQ(result.steps.size(), 3u);
    EXPECT_DOUBLE_EQ(result.steps[0].y, 1.0);
    EXPECT_DOUBLE_EQ(result.steps[1].y, 2.0);
    EXPECT_DOUBLE_EQ(result.steps[2].y, 3.0);
    EXPECT_DOUBLE_EQ(result.steps[0].u, 1.0);
    EXPECT_DOUBLE_EQ(result.steps[1].u, 2.0);
    EXPECT_DOUBLE_EQ(result.steps[2].u, 3.0);
}

TEST(SimulatorTest, NumbersStepsStartingFromZero) {
    Model35 model(1.0, 0.1);
    InputSignal signal(SignalType::Step);

    Simulator simulator(model, signal, 4);
    const SimulationResult result = simulator.run();

    ASSERT_EQ(result.steps.size(), 4u);
    for (std::size_t i = 0; i < result.steps.size(); ++i) {
        EXPECT_EQ(result.steps[i].tau, static_cast<int>(i));
    }
}

TEST(SimulatorTest, ResultContainsStabilityReport) {
    Model17 model(0.5, 1.0, 0.0, 0.0);
    InputSignal signal(SignalType::Step);

    Simulator simulator(model, signal, 3);
    const SimulationResult result = simulator.run();

    EXPECT_TRUE(result.stability.analyzed);
    EXPECT_TRUE(result.stability.stable);
    EXPECT_NEAR(result.stability.maxMagnitude, 0.5, 1e-9);
}

TEST(SimulatorTest, PrintsWarningForUnstableModel) {
    Model17 model(2.0, 1.0, 0.0, 0.0);
    InputSignal signal(SignalType::Step);

    Simulator simulator(model, signal, 3);
    std::ostringstream log;
    const SimulationResult result = simulator.run(log);

    EXPECT_FALSE(result.stability.stable);
    EXPECT_NE(log.str().find("WARNING"), std::string::npos);
    EXPECT_NE(log.str().find("НЕУСТОЙЧИВА"), std::string::npos);
    EXPECT_NE(log.str().find("Model 1.7"), std::string::npos);
}

TEST(SimulatorTest, DoesNotWarnForStableModel) {
    Model17 model(0.5, 1.0, 0.0, 0.0);
    InputSignal signal(SignalType::Step);

    Simulator simulator(model, signal, 3);
    std::ostringstream log;
    simulator.run(log);

    EXPECT_TRUE(log.str().empty());
}

TEST(SimulatorTest, DoesNotWarnForNonlinearModel) {
    Model21 model(0.5, 0.05, 0.8, 0.2);
    InputSignal signal(SignalType::Step);

    Simulator simulator(model, signal, 3);
    std::ostringstream log;
    const SimulationResult result = simulator.run(log);

    EXPECT_FALSE(result.stability.analyzed);
    EXPECT_TRUE(log.str().empty());
}

TEST(SimulatorTest, ResetsModelBeforeEachRun) {
    Model17 model(0.7, 0.5, 0.2, 0.1);
    InputSignal signal(SignalType::Step);

    Simulator simulator(model, signal, 5);
    const SimulationResult first = simulator.run();
    const SimulationResult second = simulator.run();

    ASSERT_EQ(first.steps.size(), second.steps.size());
    for (std::size_t i = 0; i < first.steps.size(); ++i) {
        EXPECT_DOUBLE_EQ(first.steps[i].y, second.steps[i].y);
    }
}

TEST(SimulatorTest, WorksWithConstantStepInput) {
    Model35 model(1.0, 0.1);
    InputSignal signal(SignalType::Step);

    Simulator simulator(model, signal, 3);
    const SimulationResult result = simulator.run();

    ASSERT_EQ(result.steps.size(), 3u);
    EXPECT_NEAR(result.steps[0].y, 0.1 * std::sin(1.0), 1e-12);
    EXPECT_NEAR(result.steps[1].y, 0.2 * std::sin(1.0), 1e-12);
    EXPECT_NEAR(result.steps[2].y, 0.3 * std::sin(1.0), 1e-12);
}

class SimulatorModeTest : public ::testing::TestWithParam<SimulatorModeCase> {
};

TEST_P(SimulatorModeTest, HandlesAllControlModes) {
    const SimulatorModeCase& testCase = GetParam();
    const InputSignal signal(testCase.type);
    Model35 model(1.0, 0.1);

    Simulator simulator(model, signal,
                        static_cast<int>(testCase.expectedY.size()));
    const SimulationResult result = simulator.run();

    ASSERT_EQ(result.steps.size(), testCase.expectedY.size());
    for (std::size_t i = 0; i < testCase.expectedY.size(); ++i) {
        EXPECT_NEAR(result.steps[i].y, testCase.expectedY[i], 1e-12)
            << "step " << i;
    }
}

INSTANTIATE_TEST_SUITE_P(
    StepImpulseHarmonic,
    SimulatorModeTest,
    ::testing::Values(
        SimulatorModeCase{SignalType::Step,
                          {0.1 * std::sin(1.0),
                           0.2 * std::sin(1.0),
                           0.3 * std::sin(1.0)}},
        SimulatorModeCase{SignalType::Impulse,
                          {0.1 * std::sin(1.0),
                           0.1 * std::sin(1.0),
                           0.1 * std::sin(1.0)}},
        SimulatorModeCase{SignalType::Harmonic,
                          {0.0,
                           0.1 * std::sin(std::sin(1.0)),
                           0.1 * std::sin(std::sin(1.0))
                               + 0.1 * std::sin(std::sin(2.0))}}));
