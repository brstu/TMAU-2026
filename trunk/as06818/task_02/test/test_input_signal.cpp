#include <gtest/gtest.h>

#include "../src/input_signal.hpp"

// Ступенчатое воздействие постоянно и равно единице.
TEST(InputSignalTest, StepIsConstantOne) {
    for (int tau = 0; tau < 5; ++tau) {
        EXPECT_DOUBLE_EQ(input_value(SignalType::Step, tau), 1.0);
    }
}

// Импульсное воздействие: единица только на нулевом шаге.
TEST(InputSignalTest, ImpulseOnlyAtZeroStep) {
    EXPECT_DOUBLE_EQ(input_value(SignalType::Impulse, 0), 1.0);
    EXPECT_DOUBLE_EQ(input_value(SignalType::Impulse, 1), 0.0);
    EXPECT_DOUBLE_EQ(input_value(SignalType::Impulse, 2), 0.0);
    EXPECT_DOUBLE_EQ(input_value(SignalType::Impulse, 10), 0.0);
}

// Гармоническое воздействие: u_tau = sin(tau).
TEST(InputSignalTest, HarmonicFollowsSine) {
    EXPECT_DOUBLE_EQ(input_value(SignalType::Harmonic, 0), 0.0);
    EXPECT_NEAR(input_value(SignalType::Harmonic, 1), 0.841470984807897, 1e-12);
    EXPECT_NEAR(input_value(SignalType::Harmonic, 2), 0.909297426825682, 1e-12);
}

// Названия типов воздействий для консоли и CSV.
TEST(InputSignalTest, NamesAreStable) {
    EXPECT_STREQ(signal_name(SignalType::Step), "step");
    EXPECT_STREQ(signal_name(SignalType::Impulse), "impulse");
    EXPECT_STREQ(signal_name(SignalType::Harmonic), "harmonic");
}
