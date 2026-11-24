#include <gtest/gtest.h>
#include <cmath>
#include "input_signal.h"

TEST(SignalTest, StepIsConstant)
{
    EXPECT_DOUBLE_EQ(InputSignal::getSignal(InputType::Step, 1, 2.5), 2.5);
    EXPECT_DOUBLE_EQ(InputSignal::getSignal(InputType::Step, 7, 2.5), 2.5);
}

TEST(SignalTest, ImpulseIsNonZeroOnlyAtFirstStep)
{
    EXPECT_DOUBLE_EQ(InputSignal::getSignal(InputType::Impulse, 1, 3.0), 3.0);
    EXPECT_DOUBLE_EQ(InputSignal::getSignal(InputType::Impulse, 2, 3.0), 0.0);
    EXPECT_DOUBLE_EQ(InputSignal::getSignal(InputType::Impulse, 5, 3.0), 0.0);
}

TEST(SignalTest, HarmonicFollowsSine)
{
    EXPECT_NEAR(InputSignal::getSignal(InputType::Harmonic, 1, 2.0),
                2.0 * std::sin(1.0), 1e-12);
    EXPECT_NEAR(InputSignal::getSignal(InputType::Harmonic, 2, 1.0),
                std::sin(2.0), 1e-12);
}

TEST(SignalTest, UnknownTypeGivesHarmonicByDefault)
{
    // в task_01 любое не-Step/Impulse значение уходит в ветку harmonic
    EXPECT_NEAR(InputSignal::getSignal(static_cast<InputType>(99), 1, 2.0),
                2.0 * std::sin(1.0), 1e-12);
}