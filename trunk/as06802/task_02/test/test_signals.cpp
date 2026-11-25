#include "StabilityAnalyzer.h"

#include <gtest/gtest.h>

#include <cmath>
#include <stdexcept>

namespace var17 {
namespace {

TEST(ControlGeneratorSuite, ProducesExpectedWaveforms) {
    const auto unitStep = buildControlGenerator(SignalKind::Step);
    for (int k : {0, 3, 12}) {
        EXPECT_DOUBLE_EQ(unitStep(k), 1.0);
    }

    const auto unitImpulse = buildControlGenerator(SignalKind::Impulse);
    EXPECT_DOUBLE_EQ(unitImpulse(0), 1.0);
    for (int k : {1, 4, 9}) {
        EXPECT_DOUBLE_EQ(unitImpulse(k), 0.0);
    }

    const auto sineWave = buildControlGenerator(SignalKind::Harmonic);
    for (int k = 0; k < 5; ++k) {
        EXPECT_NEAR(sineWave(k), std::sin(static_cast<double>(k)), 1e-12);
    }

    EXPECT_THROW(buildControlGenerator(static_cast<SignalKind>(42)), std::invalid_argument);
}

} // namespace
} // namespace var17
