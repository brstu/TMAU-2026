#include "Models.hpp"
#include <gtest/gtest.h>
#include <cmath>

TEST(ThirdOrderLinearTest, FirstOutputWithStepSignal) {
    ThirdOrderLinear m(1.0, 0.0, 0.0, 1.0);
    EXPECT_DOUBLE_EQ(m.advance(1.0), 1.0);
}

TEST(ThirdOrderLinearTest, AccumulatesStep) {
    ThirdOrderLinear m(1.0, 0.0, 0.0, 1.0);
    EXPECT_DOUBLE_EQ(m.advance(1.0), 1.0);
    EXPECT_DOUBLE_EQ(m.advance(1.0), 2.0);
    EXPECT_DOUBLE_EQ(m.advance(1.0), 3.0);
}

TEST(ThirdOrderLinearTest, ZeroInputStaysAtZero) {
    ThirdOrderLinear m(0.5, 0.3, 0.2, 1.0);
    for (int i = 0; i < 10; ++i) {
        EXPECT_DOUBLE_EQ(m.advance(0.0), 0.0);
    }
}

TEST(ThirdOrderLinearTest, ResetRestoresInitialState) {
    ThirdOrderLinear m(1.0, 0.0, 0.0, 1.0);
    m.advance(1.0);
    m.advance(1.0);
    m.reset();
    EXPECT_DOUBLE_EQ(m.advance(1.0), 1.0);
}

TEST(ThirdOrderLinearTest, DelayedImpulseResponse) {
    ThirdOrderLinear m(0.5, 0.3, 0.2, 1.0);
    EXPECT_DOUBLE_EQ(m.advance(1.0), 1.0);
    EXPECT_DOUBLE_EQ(m.advance(0.0), 0.5);
    EXPECT_DOUBLE_EQ(m.advance(0.0), 0.55);
}


TEST(LogisticMapTest, ZeroStateStaysZero) {
    LogisticMap m(1.0, 1.0, 1.0, 0.0);
    for (int i = 0; i < 10; ++i) {
        EXPECT_DOUBLE_EQ(m.advance(0.0), 0.0);
    }
}

TEST(LogisticMapTest, KnownEvolution) {
    LogisticMap m(1.0, 0.0, 0.0, 0.5);
    EXPECT_DOUBLE_EQ(m.advance(0.0), 0.25);
    EXPECT_DOUBLE_EQ(m.advance(0.0), 0.1875);
}

TEST(LogisticMapTest, CParameterAffectsOutput) {
    LogisticMap m(0.0, 0.0, 1.0, 0.5); 
    EXPECT_DOUBLE_EQ(m.advance(1.0), 0.0); 
    EXPECT_NEAR(m.advance(1.0), std::sin(0.5), 1e-12);
}

TEST(LogisticMapTest, ResetRestoresInitialY0) {
    LogisticMap m(1.0, 0.0, 0.0, 0.5);
    m.advance(0.0);
    m.advance(0.0);
    m.reset();
    EXPECT_DOUBLE_EQ(m.advance(0.0), 0.25);
}


TEST(OffsetODETest, EulerStepMatchesAnalyticOneStep) {
    OffsetODE m(1.0, 0.0, 0.1, 1.0);
    EXPECT_DOUBLE_EQ(m.advance(0.0), 0.9);
    EXPECT_DOUBLE_EQ(m.advance(0.0), 0.81);
}

TEST(OffsetODETest, BOffsetDrivesSystem) {
    OffsetODE m(1.0, 1.0, 0.1, 0.0);
    EXPECT_DOUBLE_EQ(m.advance(0.0), 0.1);
}

TEST(OffsetODETest, StepInputContributesLinearly) {
    OffsetODE m(1.0, 0.0, 0.1, 0.0);
    EXPECT_DOUBLE_EQ(m.advance(1.0), 0.1);
}

TEST(OffsetODETest, ResetRestoresY0) {
    OffsetODE m(1.0, 1.0, 0.1, 5.0);
    m.advance(1.0);
    m.advance(1.0);
    m.reset();
    EXPECT_DOUBLE_EQ(m.advance(0.0), 5.0 + 0.1 * (-5.0 + 1.0));
}