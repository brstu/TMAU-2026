#include "input_signal.h"

#include <gtest/gtest.h>

#include <cmath>
#include <string>

TEST(InputSignalTest, StepSignalIsConstantOne) {
    const StepSignal signal;
    EXPECT_DOUBLE_EQ(signal.value(0), 1.0);
    EXPECT_DOUBLE_EQ(signal.value(10), 1.0);
}

TEST(InputSignalTest, ImpulseSignalIsOneOnlyAtZero) {
    const ImpulseSignal signal;
    EXPECT_DOUBLE_EQ(signal.value(0), 1.0);
    EXPECT_DOUBLE_EQ(signal.value(1), 0.0);
    EXPECT_DOUBLE_EQ(signal.value(10), 0.0);
}

TEST(InputSignalTest, HarmonicSignalEqualsSineOfStep) {
    const HarmonicSignal signal;
    EXPECT_DOUBLE_EQ(signal.value(0), 0.0);
    EXPECT_NEAR(signal.value(1), std::sin(1.0), 1e-12);
    EXPECT_NEAR(signal.value(2), std::sin(2.0), 1e-12);
}


TEST(InputSignalTest, StepSignalReturnsItsName) {
    const StepSignal signal;
    EXPECT_NE(signal.name().find("Ступенчатый"), std::string::npos);
    EXPECT_NE(signal.name().find("u(tau) = 1"), std::string::npos);
}

TEST(InputSignalTest, ImpulseSignalReturnsItsName) {
    const ImpulseSignal signal;
    EXPECT_NE(signal.name().find("Импульсный"), std::string::npos);
    EXPECT_NE(signal.name().find("u(0) = 1"), std::string::npos);
}

TEST(InputSignalTest, HarmonicSignalReturnsItsName) {
    const HarmonicSignal signal;
    EXPECT_NE(signal.name().find("Гармонический"), std::string::npos);
    EXPECT_NE(signal.name().find("sin(tau)"), std::string::npos);
}
