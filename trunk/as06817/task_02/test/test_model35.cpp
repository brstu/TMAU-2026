#include <gtest/gtest.h>

#include <cmath>

#include "Model35.h"

namespace {
constexpr double kPi = 3.14159265358979323846;

struct Model35Case {
    double b;
    double dt;
    double u;
    int steps;
    double expected;
};
} // namespace

class Model35EulerTest : public ::testing::TestWithParam<Model35Case> {
};

TEST_P(Model35EulerTest, EulerStepMatchesFormula) {
    const Model35Case& testCase = GetParam();
    Model35 model(testCase.b, testCase.dt);

    double value = 0.0;
    for (int i = 0; i < testCase.steps; ++i) {
        value = model.step(testCase.u);
    }

    EXPECT_NEAR(value, testCase.expected, 1e-12);
}

INSTANTIATE_TEST_SUITE_P(
    InputAndCoefficients,
    Model35EulerTest,
    ::testing::Values(
        Model35Case{1.0, 0.1, kPi / 2.0, 3, 0.3},
        Model35Case{2.0, 0.05, kPi / 6.0, 2, 0.1},
        Model35Case{-1.0, 0.2, kPi / 2.0, 2, -0.4},
        Model35Case{1.0, 0.1, 0.0, 10, 0.0},
        Model35Case{1.5, 0.02, 0.7, 100, 100.0 * 0.02 * 1.5 * std::sin(0.7)}));

TEST(Model35Test, EulerMatchesAnalyticSolutionForConstantInput) {
    const double b = 2.0;
    const double dt = 0.01;
    const double u = 0.9;
    const int steps = 250;

    Model35 model(b, dt);
    for (int i = 0; i < steps; ++i) {
        model.step(u);
    }

    const double analytic = steps * dt * b * std::sin(u);
    EXPECT_NEAR(model.output(), analytic, 1e-12);
}

TEST(Model35Test, ResetClearsOutput) {
    Model35 model(1.0, 0.1);
    model.step(kPi / 2.0);

    EXPECT_NEAR(model.output(), 0.1, 1e-12);

    model.reset();
    EXPECT_DOUBLE_EQ(model.output(), 0.0);
}

TEST(Model35Test, ZeroInputKeepsSystemAtRest) {
    Model35 model(1.0, 0.1);

    EXPECT_DOUBLE_EQ(model.step(0.0), 0.0);
    EXPECT_DOUBLE_EQ(model.step(0.0), 0.0);
}

TEST(Model35Test, InitialStateIsUsedAfterReset) {
    Model35 model(1.0, 0.1, 2.5);

    EXPECT_DOUBLE_EQ(model.output(), 2.5);
    EXPECT_NEAR(model.step(kPi / 2.0), 2.6, 1e-12);

    model.reset();
    EXPECT_DOUBLE_EQ(model.output(), 2.5);
}

TEST(Model35Test, ExposesNameAndHasNoCharacteristicPolynomial) {
    const Model35 model(1.0, 0.1);

    EXPECT_EQ(model.name(), "Model 3.5");
    EXPECT_FALSE(model.isLinear());
    EXPECT_TRUE(model.characteristicPolynomial().empty());
}
