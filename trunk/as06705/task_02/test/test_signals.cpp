#include "InputSignals.hpp"
#include <gtest/gtest.h>
#include <cmath>

TEST(StepSignalTest, ConstantEverywhere) {
    StepSignal s(2.0);
    EXPECT_DOUBLE_EQ(s.at(0), 2.0);
    EXPECT_DOUBLE_EQ(s.at(1), 2.0);
    EXPECT_DOUBLE_EQ(s.at(100), 2.0);
}

TEST(StepSignalTest, DefaultAmplitude) {
    StepSignal s;
    EXPECT_DOUBLE_EQ(s.at(5), 1.0);
}

TEST(StepSignalTest, Id) {
    StepSignal s;
    EXPECT_STREQ(s.id(), "step");
}

TEST(ImpulseSignalTest, NonZeroOnlyAtZero) {
    ImpulseSignal s(3.0);
    EXPECT_DOUBLE_EQ(s.at(0), 3.0);
    EXPECT_DOUBLE_EQ(s.at(1), 0.0);
    EXPECT_DOUBLE_EQ(s.at(100), 0.0);
}

TEST(ImpulseSignalTest, Id) {
    ImpulseSignal s;
    EXPECT_STREQ(s.id(), "impulse");
}

TEST(HarmonicSignalTest, MatchesSin) {
    HarmonicSignal s(1.0);
    for (int t = 0; t < 10; ++t) {
        EXPECT_NEAR(s.at(t), std::sin(static_cast<double>(t)), 1e-12);
    }
}

TEST(HarmonicSignalTest, AmplitudeScales) {
    HarmonicSignal s(2.0);
    EXPECT_NEAR(s.at(1), 2.0 * std::sin(1.0), 1e-12);
}

TEST(HarmonicSignalTest, Id) {
    HarmonicSignal s;
    EXPECT_STREQ(s.id(), "harmonic");
}

TEST(MakeSignalTest, CreatesCorrectType) {
    EXPECT_STREQ(makeSignal(1)->id(), "step");
    EXPECT_STREQ(makeSignal(2)->id(), "impulse");
    EXPECT_STREQ(makeSignal(3)->id(), "harmonic");
}

TEST(MakeSignalTest, InvalidChoiceReturnsNull) {
    EXPECT_EQ(makeSignal(0), nullptr);
    EXPECT_EQ(makeSignal(99), nullptr);
    EXPECT_EQ(makeSignal(-1), nullptr);
}