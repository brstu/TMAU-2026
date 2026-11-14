#include <gtest/gtest.h>

#include <cmath>

#include "Signal.h"

TEST(SignalTest, StepIsConstant)
{
    EXPECT_DOUBLE_EQ(getInput(SignalType::Step, 0, 2.5), 2.5);
    EXPECT_DOUBLE_EQ(getInput(SignalType::Step, 7, 2.5), 2.5);
}

TEST(SignalTest, ImpulseIsNonZeroOnlyAtStart)
{
    EXPECT_DOUBLE_EQ(getInput(SignalType::Impulse, 0, 3.0), 3.0);
    EXPECT_DOUBLE_EQ(getInput(SignalType::Impulse, 1, 3.0), 0.0);
    EXPECT_DOUBLE_EQ(getInput(SignalType::Impulse, 5, 3.0), 0.0);
}

TEST(SignalTest, HarmonicFollowsSine)
{
    EXPECT_DOUBLE_EQ(getInput(SignalType::Harmonic, 0, 2.0), 0.0);
    EXPECT_NEAR(getInput(SignalType::Harmonic, 1, 2.0), 2.0 * std::sin(1.0), 1e-12);
    EXPECT_NEAR(getInput(SignalType::Harmonic, 2, 1.0), std::sin(2.0), 1e-12);
}

TEST(SignalTest, UnknownTypeGivesZero)
{
    EXPECT_DOUBLE_EQ(getInput(static_cast<SignalType>(99), 3, 5.0), 0.0);
}
