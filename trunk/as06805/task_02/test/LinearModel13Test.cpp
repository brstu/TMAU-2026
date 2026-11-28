#include <gtest/gtest.h>

#include "LinearModel13.h"

#include <cmath>

namespace {

struct LinearModel13Params {
    double a1;
    double a2;
    double b;
    double u;
};

class LinearModel13ParamTest : public ::testing::TestWithParam<LinearModel13Params> {};

// Первый шаг при нулевом начальном состоянии: y(1) = b * u(0).
TEST_P(LinearModel13ParamTest, FirstStepEqualsBtimesU) {
    const LinearModel13Params p = GetParam();
    LinearModel13 model(p.a1, p.a2, p.b);

    EXPECT_NEAR(p.b * p.u, model.step(p.u), 1e-12);
}

// Второй шаг: y(2) = a1*y(1) + a2*y(0) + b*u(1).
TEST_P(LinearModel13ParamTest, SecondStepFollowsRecurrence) {
    const LinearModel13Params p = GetParam();
    LinearModel13 model(p.a1, p.a2, p.b);

    const double y1 = model.step(p.u);
    const double y2 = model.step(p.u);

    EXPECT_NEAR(p.a1 * y1 + p.a2 * 0.0 + p.b * p.u, y2, 1e-12);
}

// Третий шаг учитывает запаздывающую компоненту y(tau-1).
TEST_P(LinearModel13ParamTest, ThirdStepUsesDelayedComponent) {
    const LinearModel13Params p = GetParam();
    LinearModel13 model(p.a1, p.a2, p.b);

    const double y1 = model.step(p.u);
    const double y2 = model.step(p.u);
    const double y3 = model.step(p.u);

    EXPECT_NEAR(p.a1 * y2 + p.a2 * y1 + p.b * p.u, y3, 1e-12);
}

INSTANTIATE_TEST_SUITE_P(
    WithDifferentCoefficients,
    LinearModel13ParamTest,
    ::testing::Values(LinearModel13Params{0.5, 0.1, 1.0, 1.0},
                       LinearModel13Params{0.5, -0.2, 0.5, -2.0},
                       LinearModel13Params{0.0, 0.0, 2.0, 0.25},
                       LinearModel13Params{-0.4, 0.3, 1.0, 3.0}));

}  // namespace

// Проверка значений, рассчитанных вручную (a1=0.5, a2=0.1, b=1.0, u=1).
TEST(LinearModel13Test, ManualStepByStepValues) {
    LinearModel13 model(0.5, 0.1, 1.0);

    EXPECT_NEAR(1.0000, model.step(1.0), 1e-12);
    EXPECT_NEAR(1.5000, model.step(1.0), 1e-12);
    EXPECT_NEAR(1.8500, model.step(1.0), 1e-12);
    EXPECT_NEAR(2.0750, model.step(1.0), 1e-12);
    EXPECT_NEAR(2.2225, model.step(1.0), 1e-12);
}

// Корни характеристического уравнения z^2 - a1*z - a2 = 0.
TEST(LinearModel13Test, PolesMatchAnalyticalRoots) {
    LinearModel13 model(0.5, 0.1, 1.0);
    const auto poles = model.poles();

    ASSERT_EQ(2u, poles.size());

    const double sq = std::sqrt(0.65);
    EXPECT_NEAR((0.5 + sq) / 2.0, poles[0].real(), 1e-12);
    EXPECT_NEAR((0.5 - sq) / 2.0, poles[1].real(), 1e-12);
    EXPECT_NEAR(0.0, poles[0].imag(), 1e-12);
    EXPECT_NEAR(0.0, poles[1].imag(), 1e-12);
}

// Комплексно-сопряжённые полюса при a1 = 0, a2 = -0.5: z = +/- j*sqrt(0.5).
TEST(LinearModel13Test, ComplexConjugatePoles) {
    LinearModel13 model(0.0, -0.5, 1.0);
    const auto poles = model.poles();

    ASSERT_EQ(2u, poles.size());
    EXPECT_NEAR(0.0, poles[0].real(), 1e-12);
    EXPECT_NEAR(std::sqrt(0.5), std::abs(poles[0].imag()), 1e-12);
    EXPECT_NEAR(std::sqrt(0.5), std::abs(poles[1].imag()), 1e-12);
    EXPECT_NEAR(poles[0].imag(), -poles[1].imag(), 1e-12);
}

// Инвариант: нулевой вход и нулевое состояние => покой.
TEST(LinearModel13Test, ZeroInputAndZeroStateStayAtRest) {
    LinearModel13 model(0.5, 0.1, 1.0);

    for (int i = 0; i < 10; ++i) {
        EXPECT_DOUBLE_EQ(0.0, model.step(0.0));
    }
}

// Сброс обнуляет обе компоненты состояния.
TEST(LinearModel13Test, ResetRestoresBothStateComponents) {
    LinearModel13 model(0.5, 0.1, 1.0);
    model.step(1.0);
    model.step(1.0);

    model.reset();
    EXPECT_DOUBLE_EQ(0.0, model.step(0.0));
    EXPECT_DOUBLE_EQ(0.0, model.step(0.0));
}

// Начальное состояние y(tau), y(tau-1) используется при расчёте шага.
TEST(LinearModel13Test, SetInitialStateIsUsedInFirstStep) {
    LinearModel13 model(0.5, 0.1, 1.0);
    model.setInitialState({2.0, -1.0});

    EXPECT_NEAR(0.5 * 2.0 + 0.1 * (-1.0) + 1.0, model.step(1.0), 1e-12);
}

// Частично заданное начальное состояние: отсутствующая компонента равна нулю.
TEST(LinearModel13Test, PartialInitialStateFillsMissingComponentWithZero) {
    LinearModel13 model(0.5, 0.1, 1.0);
    model.setInitialState({2.0});

    EXPECT_NEAR(0.5 * 2.0 + 0.1 * 0.0 + 1.0, model.step(1.0), 1e-12);
}

// Имя и описание модели.
TEST(LinearModel13Test, NameAndEquationAreInformative) {
    LinearModel13 model(0.5, 0.1, 1.0);

    EXPECT_EQ("Model 1.3 (Linear 2nd order)", model.name());
    EXPECT_NE(std::string::npos, model.equation().find("y(tau+1)"));
}
