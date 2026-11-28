#include "StabilityAnalyzer.h"

#include <gtest/gtest.h>

#include <stdexcept>

namespace task02 {
namespace {

TEST(InputSignalTest, GeneratesStandardSignals) {
    EXPECT_DOUBLE_EQ(makeInputSignal(InputType::Step)(5), 1.0);
    const auto impulse = makeInputSignal(InputType::Impulse);
    EXPECT_DOUBLE_EQ(impulse(0), 1.0);
    EXPECT_DOUBLE_EQ(impulse(3), 0.0);
    EXPECT_NEAR(makeInputSignal(InputType::Harmonic)(1), 0.8414709848078965, 1e-12);
    EXPECT_THROW(makeInputSignal(static_cast<InputType>(0)), std::invalid_argument);
}

} // namespace
} // namespace task02
