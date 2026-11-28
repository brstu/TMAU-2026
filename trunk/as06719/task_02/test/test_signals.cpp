#include <gtest/gtest.h>
#include "E:/TMAU-2026/trunk/as06719/task_02/src/signals.h"

TEST(SignalsTest, GeneratesStepSignal) {
    auto signal = stepSignal(4, 2.0);

    ASSERT_EQ(signal.size(), 4);
    for (double value : signal) {
        EXPECT_DOUBLE_EQ(value, 2.0);
    }
}

TEST(SignalsTest, GeneratesImpulseSignal) {
    auto signal = impulseSignal(4, 1.0);

    ASSERT_EQ(signal.size(), 4);
    EXPECT_DOUBLE_EQ(signal[0], 1.0);
    EXPECT_DOUBLE_EQ(signal[1], 0.0);
    EXPECT_DOUBLE_EQ(signal[2], 0.0);
    EXPECT_DOUBLE_EQ(signal[3], 0.0);
}

TEST(SignalsTest, GeneratesHarmonicSignal) {
    auto signal = harmonicSignal(4, 1.0, 0.25);

    ASSERT_EQ(signal.size(), 4);
    EXPECT_NEAR(signal[0], 0.0, 1e-10);
    EXPECT_NEAR(signal[1], 1.0, 1e-10);
    EXPECT_NEAR(signal[2], 0.0, 1e-10);
    EXPECT_NEAR(signal[3], -1.0, 1e-10);
}