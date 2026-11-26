#include <gtest/gtest.h>

#include "SignalGenerator.h"

#include <cmath>

namespace {

constexpr double kPi = 3.14159265358979323846;

struct SignalParams {
    SignalType type;
    double amplitude;
    double tau;
    double expected;
};

class SignalGeneratorParamTest : public ::testing::TestWithParam<SignalParams> {};

// Значение сигнала в заданный момент времени для всех типов воздействий.
TEST_P(SignalGeneratorParamTest, SignalValueAtGivenTime) {
    const SignalParams p = GetParam();
    SignalGenerator generator(p.type, p.amplitude);

    EXPECT_NEAR(p.expected, generator.get(p.tau), 1e-12);
}

INSTANTIATE_TEST_SUITE_P(
    ForAllSignalTypes,
    SignalGeneratorParamTest,
    ::testing::Values(
        // Ступенчатое воздействие.
        SignalParams{SignalType::STEP, 1.0, 0.0, 1.0},
        SignalParams{SignalType::STEP, 2.5, 17.0, 2.5},
        SignalParams{SignalType::STEP, -1.0, 3.0, -1.0},
        // Импульсное воздействие.
        SignalParams{SignalType::IMPULSE, 1.0, 0.0, 1.0},
        SignalParams{SignalType::IMPULSE, 1.0, 1.0, 0.0},
        SignalParams{SignalType::IMPULSE, 3.0, 42.0, 0.0},
        // Гармоническое воздействие.
        SignalParams{SignalType::HARMONIC, 1.0, 0.0, 0.0},
        SignalParams{SignalType::HARMONIC, 2.0, kPi / 2.0, 2.0}));

}  // namespace

// Ступенчатое воздействие постоянно на всей интервале.
TEST(SignalGeneratorTest, StepSignalIsConstant) {
    SignalGenerator generator(SignalType::STEP, 1.5);

    for (int tau = 0; tau < 20; ++tau) {
        EXPECT_DOUBLE_EQ(1.5, generator.get(static_cast<double>(tau)));
    }
}

// Импульсное воздействие ненулевое только в нулевой момент времени.
TEST(SignalGeneratorTest, ImpulseSignalIsNonZeroOnlyAtZero) {
    SignalGenerator generator(SignalType::IMPULSE, 2.0);

    EXPECT_DOUBLE_EQ(2.0, generator.get(0.0));
    EXPECT_DOUBLE_EQ(0.0, generator.get(1e-3));
    EXPECT_DOUBLE_EQ(0.0, generator.get(10.0));
}

// Граница отсечки импульса: значения меньше 1e-9 трактуются как tau = 0.
TEST(SignalGeneratorTest, ImpulseSignalBoundaryNearZero) {
    SignalGenerator generator(SignalType::IMPULSE, 1.0);

    EXPECT_DOUBLE_EQ(1.0, generator.get(1e-12));
    EXPECT_DOUBLE_EQ(0.0, generator.get(1e-6));
}

// Гармоническое воздействие: u(tau) = A*sin(tau).
TEST(SignalGeneratorTest, HarmonicSignalFollowsSine) {
    SignalGenerator generator(SignalType::HARMONIC, 2.0);

    EXPECT_NEAR(0.0, generator.get(0.0), 1e-12);
    EXPECT_NEAR(2.0, generator.get(kPi / 2.0), 1e-12);
    EXPECT_NEAR(0.0, generator.get(kPi), 1e-12);
    EXPECT_NEAR(-2.0, generator.get(3.0 * kPi / 2.0), 1e-12);
}

// Амплитуда и тип сигнала доступны через геттеры.
TEST(SignalGeneratorTest, TypeAndAmplitudeAreAccessible) {
    SignalGenerator generator(SignalType::HARMONIC, 3.5);

    EXPECT_EQ(SignalType::HARMONIC, generator.getType());
    EXPECT_DOUBLE_EQ(3.5, generator.getAmplitude());
}

// Текстовые имена типов сигналов.
TEST(SignalGeneratorTest, TypeNamesAreCorrect) {
    EXPECT_EQ("step", SignalGenerator::typeName(SignalType::STEP));
    EXPECT_EQ("impulse", SignalGenerator::typeName(SignalType::IMPULSE));
    EXPECT_EQ("harmonic", SignalGenerator::typeName(SignalType::HARMONIC));
}

// Значения по умолчанию: ступенчатое воздействие единичной амплитуды.
TEST(SignalGeneratorTest, DefaultConstructorUsesStepSignal) {
    SignalGenerator generator;

    EXPECT_EQ(SignalType::STEP, generator.getType());
    EXPECT_DOUBLE_EQ(1.0, generator.getAmplitude());
    EXPECT_DOUBLE_EQ(1.0, generator.get(5.0));
}
