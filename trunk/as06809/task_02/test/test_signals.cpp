#include "StabilityAnalyzer.h"

#include <gtest/gtest.h>

#include <cmath>
#include <stdexcept>

namespace var16 {
namespace {

TEST(InputFnGeneratorTest, VerifiesStepImpulseAndSineWaveforms) {
    const auto stepGen = createInputFn(InputMode::Step);
    EXPECT_DOUBLE_EQ(stepGen(0), 1.0);
    EXPECT_DOUBLE_EQ(stepGen(15), 1.0);

    const auto impulseGen = createInputFn(InputMode::Impulse);
    EXPECT_DOUBLE_EQ(impulseGen(0), 1.0);
    EXPECT_DOUBLE_EQ(impulseGen(1), 0.0);
    EXPECT_DOUBLE_EQ(impulseGen(8), 0.0);

    const auto sineGen = createInputFn(InputMode::Harmonic);
    for (int t = 0; t <= 4; ++t) {
        EXPECT_NEAR(sineGen(t), std::sin(static_cast<double>(t)), 1e-12);
    }

    EXPECT_THROW(createInputFn(static_cast<InputMode>(-1)), std::invalid_argument);
}

} // namespace
} // namespace var16
