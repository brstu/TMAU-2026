#include <gtest/gtest.h>

#include <cmath>
#include <stdexcept>

#include "InputSignal.h"

TEST(SignalTest, StepSignalIsConstant) {
    StepSignal signal;
    EXPECT_DOUBLE_EQ(signal.value(0, 0.0), 1.0);
    EXPECT_DOUBLE_EQ(signal.value(5, 0.5), 1.0);
}

TEST(SignalTest, ImpulseSignalIsNonZeroOnlyAtFirstSample) {
    ImpulseSignal signal;
    EXPECT_DOUBLE_EQ(signal.value(0, 0.0), 1.0);
    EXPECT_DOUBLE_EQ(signal.value(1, 0.1), 0.0);
    EXPECT_DOUBLE_EQ(signal.value(10, 1.0), 0.0);
}

TEST(SignalTest, HarmonicSignalMatchesSineOfTime) {
    HarmonicSignal signal;
    EXPECT_NEAR(signal.value(0, 0.0), 0.0, 1e-12);
    EXPECT_NEAR(signal.value(5, 0.7), std::sin(0.7), 1e-12);
    EXPECT_NEAR(signal.value(7, 2.3), std::sin(2.3), 1e-12);
}

TEST(SignalTest, FactoryCreatesEverySupportedSignal) {
    for (int type = 1; type <= 3; ++type) {
        EXPECT_NE(makeSignal(type), nullptr) << "type = " << type;
    }
}

TEST(SignalTest, FactoryRejectsUnknownSignalType) {
    EXPECT_THROW(makeSignal(0), std::invalid_argument);
    EXPECT_THROW(makeSignal(4), std::invalid_argument);
}

TEST(SignalTest, FactorySignalsKeepTheirDefinitions) {
    auto step = makeSignal(1);
    auto impulse = makeSignal(2);
    auto harmonic = makeSignal(3);

    EXPECT_DOUBLE_EQ(step->value(3, 0.3), 1.0);
    EXPECT_DOUBLE_EQ(impulse->value(0, 0.0), 1.0);
    EXPECT_DOUBLE_EQ(impulse->value(3, 0.3), 0.0);
    EXPECT_NEAR(harmonic->value(3, 0.3), std::sin(0.3), 1e-12);
}
