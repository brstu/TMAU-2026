#include <gtest/gtest.h>

#include <cmath>

#include "InputSignal.h"

namespace {
constexpr double kPi = 3.14159265358979323846;

struct SignalCase {
    SignalType type;
    double amplitude;
    double frequency;
    double phase;
    int tau;
    double expected;
};
} // namespace

class InputSignalParameterizedTest
    : public ::testing::TestWithParam<SignalCase> {
};

TEST_P(InputSignalParameterizedTest, ReturnsExpectedValue) {
    const SignalCase& testCase = GetParam();
    const InputSignal signal(testCase.type,
                             testCase.amplitude,
                             testCase.frequency,
                             testCase.phase);

    EXPECT_NEAR(signal.value(testCase.tau), testCase.expected, 1e-12);
}

INSTANTIATE_TEST_SUITE_P(
    AllModes,
    InputSignalParameterizedTest,
    ::testing::Values(
        SignalCase{SignalType::Step, 1.0, 1.0, 0.0, 0, 1.0},
        SignalCase{SignalType::Step, 2.5, 1.0, 0.0, 10, 2.5},
        SignalCase{SignalType::Impulse, 1.0, 1.0, 0.0, 0, 1.0},
        SignalCase{SignalType::Impulse, 1.0, 1.0, 0.0, 1, 0.0},
        SignalCase{SignalType::Impulse, 3.0, 1.0, 0.0, 0, 3.0},
        SignalCase{SignalType::Harmonic, 1.0, 1.0, 0.0, 0, 0.0},
        SignalCase{SignalType::Harmonic, 1.0, 1.0, 0.0, 1, std::sin(1.0)},
        SignalCase{SignalType::Harmonic, 2.0, 2.0, 0.0, 1, 2.0 * std::sin(2.0)},
        SignalCase{SignalType::Harmonic, 1.0, 1.0, kPi / 2.0, 0, 1.0}));

TEST(InputSignalTest, StoresSignalType) {
    const InputSignal signal(SignalType::Harmonic);
    EXPECT_EQ(signal.type(), SignalType::Harmonic);
}

TEST(InputSignalTest, DefaultAmplitudeIsOne) {
    const InputSignal signal(SignalType::Step);
    EXPECT_DOUBLE_EQ(signal.amplitude(), 1.0);
    EXPECT_DOUBLE_EQ(signal.value(0), 1.0);
}

TEST(InputSignalTest, HarmonicWithFrequencyTwoPiIsPeriodic) {
    const InputSignal signal(SignalType::Harmonic, 1.0, 2.0 * kPi);

    EXPECT_NEAR(signal.value(0), 0.0, 1e-12);
    EXPECT_NEAR(signal.value(1), 0.0, 1e-12);
    EXPECT_NEAR(signal.value(2), 0.0, 1e-12);
}

TEST(InputSignalTest, StepKeepsConstantValueForAllSteps) {
    const InputSignal signal(SignalType::Step, 4.0);

    for (int tau = 0; tau < 10; ++tau) {
        EXPECT_DOUBLE_EQ(signal.value(tau), 4.0);
    }
}

TEST(InputSignalTest, UnknownSignalTypeReturnsZero) {
    const InputSignal signal(static_cast<SignalType>(99));
    EXPECT_DOUBLE_EQ(signal.value(0), 0.0);
}
