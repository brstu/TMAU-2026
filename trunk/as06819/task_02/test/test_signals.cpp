#include "StabilityAnalyzer.h"

#include <gtest/gtest.h>

#include <cmath>
#include <stdexcept>
#include <vector>

namespace task02 {
namespace {

TEST(SignalGeneratorSuite, StepSignalRemainsConstantUnit) {
    const auto stepFn = makeInputSignal(InputType::Step);
    for (int tau : {0, 1, 7, 25}) {
        EXPECT_DOUBLE_EQ(stepFn(tau), 1.0);
    }
}

TEST(SignalGeneratorSuite, ImpulseSignalFiresOnlyAtOrigin) {
    const auto impulseFn = makeInputSignal(InputType::Impulse);
    EXPECT_DOUBLE_EQ(impulseFn(0), 1.0);
    for (int tau : {1, 2, 5, 10}) {
        EXPECT_DOUBLE_EQ(impulseFn(tau), 0.0);
    }
}

TEST(SignalGeneratorSuite, HarmonicSignalMatchesSineWave) {
    const auto harmonicFn = makeInputSignal(InputType::Harmonic);
    for (int tau = 0; tau < 6; ++tau) {
        EXPECT_NEAR(harmonicFn(tau), std::sin(static_cast<double>(tau)), 1e-12);
    }
}

TEST(SignalGeneratorSuite, ThrowsOnUnsupportedInputType) {
    EXPECT_THROW(makeInputSignal(static_cast<InputType>(99)), std::invalid_argument);
}

} // namespace
} // namespace task02
