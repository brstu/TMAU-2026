#include <gtest/gtest.h>

#include "ODEModel37.h"

#include <cmath>

namespace {

struct EulerParams {
    double a;
    double b;
    double dt;
    double u;
};

class EulerParamTest : public ::testing::TestWithParam<EulerParams> {};

// Один шаг метода Эйлера: y(1) = y(0) + dt*(-exp(a)*y(0) + b*u(0)).
TEST_P(EulerParamTest, SingleStepFollowsEulerFormula) {
    const EulerParams p = GetParam();
    ODEModel37 model(p.a, p.b, p.dt);

    EXPECT_NEAR(0.0 + p.dt * (-std::exp(p.a) * 0.0 + p.b * p.u), model.step(p.u), 1e-12);
}

// Второй шаг учитывает изменившееся состояние.
TEST_P(EulerParamTest, SecondStepUsesUpdatedState) {
    const EulerParams p = GetParam();
    ODEModel37 model(p.a, p.b, p.dt);

    const double y1 = model.step(p.u);
    const double y2 = model.step(p.u);

    EXPECT_NEAR(y1 + p.dt * (-std::exp(p.a) * y1 + p.b * p.u), y2, 1e-12);
}

INSTANTIATE_TEST_SUITE_P(
    WithDifferentParameters,
    EulerParamTest,
    ::testing::Values(EulerParams{0.1, 1.0, 0.1, 1.0},
                       EulerParams{0.0, 2.0, 0.05, -1.0},
                       EulerParams{0.5, 0.5, 0.01, 3.0},
                       EulerParams{-0.2, 1.5, 0.2, 0.0}));

}  // namespace

// Сравнение с аналитическим решением dy/dt = -exp(a)*y + b*u при u = const.
TEST(ODEModel37Test, StepResponseConvergesToAnalyticalSolution) {
    const double a = 0.1;
    const double b = 1.0;
    const double dt = 0.01;
    const double u = 1.0;
    const double tEnd = 1.0;

    ODEModel37 model(a, b, dt);
    const int steps = static_cast<int>(std::lround(tEnd / dt));
    double y = 0.0;
    for (int i = 0; i < steps; ++i) {
        y = model.step(u);
    }

    const double k = std::exp(a);
    const double exact = (b * u / k) * (1.0 - std::exp(-k * tEnd));

    EXPECT_NEAR(exact, y, 2e-2);
}

// Порядок сходимости метода Эйлера — первый.
TEST(ODEModel37Test, ErrorDecreasesLinearlyWithStepSize) {
    const double a = 0.1;
    const double b = 1.0;
    const double u = 1.0;
    const double tEnd = 1.0;
    const double k = std::exp(a);
    const double exact = (b * u / k) * (1.0 - std::exp(-k * tEnd));

    auto eulerValue = [&](double dt) {
        ODEModel37 model(a, b, dt);
        const int steps = static_cast<int>(std::lround(tEnd / dt));
        double y = 0.0;
        for (int i = 0; i < steps; ++i) {
            y = model.step(u);
        }
        return y;
    };

    const double errCoarse = std::abs(eulerValue(0.1) - exact);
    const double errFine = std::abs(eulerValue(0.05) - exact);
    ASSERT_GT(errCoarse, 0.0);

    const double ratio = errCoarse / errFine;
    EXPECT_NEAR(2.0, ratio, 0.4);
}

// Дискретный полюс метода Эйлера: z = 1 - dt*exp(a).
TEST(ODEModel37Test, DiscretePoleMatchesEulerDiscretization) {
    ODEModel37 model(0.1, 1.0, 0.1);
    const auto poles = model.poles();

    ASSERT_EQ(1u, poles.size());
    EXPECT_NEAR(1.0 - 0.1 * std::exp(0.1), poles[0].real(), 1e-12);
    EXPECT_DOUBLE_EQ(0.0, poles[0].imag());
}

// Инвариант: нулевой вход и нулевое состояние => покой.
TEST(ODEModel37Test, ZeroInputAndZeroStateStayAtRest) {
    ODEModel37 model(0.1, 1.0, 0.1);

    for (int i = 0; i < 10; ++i) {
        EXPECT_DOUBLE_EQ(0.0, model.step(0.0));
    }
}

// Сброс состояния.
TEST(ODEModel37Test, ResetRestoresZeroState) {
    ODEModel37 model(0.1, 1.0, 0.1);
    model.step(1.0);

    model.reset();
    EXPECT_DOUBLE_EQ(0.0, model.step(0.0));
}

// Начальное состояние учитывается в первом шаге.
TEST(ODEModel37Test, SetInitialStateIsUsedInFirstStep) {
    ODEModel37 model(0.0, 1.0, 0.1);
    model.setInitialState({2.0});

    EXPECT_NEAR(2.0 + 0.1 * (-1.0 * 2.0 + 1.0), model.step(1.0), 1e-12);
}

// Коэффициент затухания непрерывной модели.
TEST(ODEModel37Test, DecayCoefficientIsExpOfA) {
    ODEModel37 model(0.25, 1.0, 0.1);

    EXPECT_NEAR(std::exp(0.25), model.decay(), 1e-12);
}

// Имя, описание и параметры модели.
TEST(ODEModel37Test, NameEquationAndParameters) {
    ODEModel37 model(0.1, 1.0, 0.1);

    EXPECT_EQ("Model 3.7 (ODE, Euler)", model.name());
    EXPECT_NE(std::string::npos, model.equation().find("dy/dt"));
    EXPECT_DOUBLE_EQ(0.1, model.a());
    EXPECT_DOUBLE_EQ(1.0, model.b());
    EXPECT_DOUBLE_EQ(0.1, model.dt());
}
