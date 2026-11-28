#include <gtest/gtest.h>

#include "NonlinearModel25.h"

#include <cmath>

namespace {

struct FrictionParams {
    double u;
    double expected;
};

class FrictionParamTest : public ::testing::TestWithParam<FrictionParams> {};

// Знаковая экспоненциальная нелинейность для положительных и отрицательных u.
TEST_P(FrictionParamTest, FrictionReturnsExpectedValue) {
    const FrictionParams p = GetParam();
    NonlinearModel25 model(0.8, 0.5);

    EXPECT_NEAR(p.expected, model.friction(p.u), 1e-12);
}

INSTANTIATE_TEST_SUITE_P(
    WithDifferentInputs,
    FrictionParamTest,
    ::testing::Values(
        // Граничная точка u = 0: нелинейность равна нулю.
        FrictionParams{0.0, 0.0},
        // Положительные входные сигналы.
        FrictionParams{1.0, 1.0 - std::exp(-1.0)},
        FrictionParams{0.5, 1.0 - std::exp(-0.5)},
        // Отрицательные входные сигналы.
        FrictionParams{-1.0, -(1.0 - std::exp(-1.0))},
        FrictionParams{-0.5, -(1.0 - std::exp(-0.5))}));

}  // namespace

// Первый шаг: y(1) = a*y(0) + b*f(u(0)).
TEST(NonlinearModel25Test, FirstStepFollowsRecurrence) {
    NonlinearModel25 model(0.8, 0.5);

    EXPECT_NEAR(0.5 * (1.0 - std::exp(-1.0)), model.step(1.0), 1e-12);
    EXPECT_NEAR(0.8 * (0.5 * (1.0 - std::exp(-1.0))) + 0.5 * (1.0 - std::exp(-2.0)),
                model.step(2.0), 1e-12);
}

// При u = 0 нелинейность равна нулю (трение покоя).
TEST(NonlinearModel25Test, ZeroInputProducesNoFriction) {
    NonlinearModel25 model(0.8, 0.5);

    EXPECT_DOUBLE_EQ(0.0, model.friction(0.0));
    EXPECT_DOUBLE_EQ(0.0, model.step(0.0));
}

// Нечётная симметрия нелинейности по отношению к знаку входного сигнала.
TEST(NonlinearModel25Test, FrictionIsOddFunction) {
    NonlinearModel25 model(0.8, 0.5);

    for (double u : {0.1, 0.7, 2.5}) {
        EXPECT_NEAR(-model.friction(u), model.friction(-u), 1e-12);
    }
}

// При |u| -> бесконечность нелинейность стремится к +/-1.
TEST(NonlinearModel25Test, FrictionSaturatesForLargeInput) {
    NonlinearModel25 model(0.8, 0.5);

    EXPECT_NEAR(1.0, model.friction(50.0), 1e-9);
    EXPECT_NEAR(-1.0, model.friction(-50.0), 1e-9);
}

// Инвариант: нулевой вход и нулевое состояние => покой.
TEST(NonlinearModel25Test, ZeroInputAndZeroStateStayAtRest) {
    NonlinearModel25 model(0.8, 0.5);

    for (int i = 0; i < 10; ++i) {
        EXPECT_DOUBLE_EQ(0.0, model.step(0.0));
    }
}

// Сброс состояния.
TEST(NonlinearModel25Test, ResetRestoresZeroState) {
    NonlinearModel25 model(0.8, 0.5);
    model.step(1.0);

    model.reset();
    EXPECT_DOUBLE_EQ(0.0, model.step(0.0));
}

// Начальное состояние учитывается в первом шаге.
TEST(NonlinearModel25Test, SetInitialStateIsUsedInFirstStep) {
    NonlinearModel25 model(0.8, 0.5);
    model.setInitialState({2.0});

    EXPECT_NEAR(0.8 * 2.0 + 0.5 * (1.0 - std::exp(-1.0)), model.step(1.0), 1e-12);
}

// Полюс нелинейной модели равен коэффициенту a.
TEST(NonlinearModel25Test, PoleEqualsCoefficientA) {
    NonlinearModel25 model(0.8, 0.5);
    const auto poles = model.poles();

    ASSERT_EQ(1u, poles.size());
    EXPECT_DOUBLE_EQ(0.8, poles[0].real());
}

// Имя и описание модели.
TEST(NonlinearModel25Test, NameAndEquationAreInformative) {
    NonlinearModel25 model(0.8, 0.5);

    EXPECT_EQ("Model 2.5 (Nonlinear friction)", model.name());
    EXPECT_NE(std::string::npos, model.equation().find("sign(u(tau))"));
}
