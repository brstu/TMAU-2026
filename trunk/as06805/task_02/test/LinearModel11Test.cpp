#include <gtest/gtest.h>

#include <cmath>

#include "LinearModel11.h"

namespace {

struct LinearModel11Params {
    double a;
    double b;
    double u;
};

class LinearModel11ParamTest : public ::testing::TestWithParam<LinearModel11Params> {};

// Проверка первого шага: y(1) = b * u(0) (нулевое начальное состояние).
TEST_P(LinearModel11ParamTest, FirstStepEqualsBtimesU) {
    const LinearModel11Params p = GetParam();
    LinearModel11 model(p.a, p.b);

    EXPECT_NEAR(p.b * p.u, model.step(p.u), 1e-12);
}

// Проверка второго шага по разностному уравнению.
TEST_P(LinearModel11ParamTest, SecondStepFollowsRecurrence) {
    const LinearModel11Params p = GetParam();
    LinearModel11 model(p.a, p.b);

    const double y1 = model.step(p.u);
    const double y2 = model.step(p.u);

    EXPECT_NEAR(p.a * y1 + p.b * p.u, y2, 1e-12);
}

INSTANTIATE_TEST_SUITE_P(
    WithDifferentCoefficients,
    LinearModel11ParamTest,
    ::testing::Values(LinearModel11Params{0.5, 1.0, 1.0},
                       LinearModel11Params{0.8, 0.5, 2.0},
                       LinearModel11Params{-0.3, 2.0, -1.5},
                       LinearModel11Params{0.0, 1.0, 1.0},
                       LinearModel11Params{0.99, 0.1, 0.5}));

}  // namespace

// Инвариант физического смысла: нулевой вход и нулевое состояние => покой.
TEST(LinearModel11Test, ZeroInputAndZeroStateStayAtRest) {
    LinearModel11 model(0.5, 1.0);

    for (int i = 0; i < 10; ++i) {
        EXPECT_DOUBLE_EQ(0.0, model.step(0.0));
    }
}

// Сброс состояния возвращает модель в нулевое состояние.
TEST(LinearModel11Test, ResetRestoresZeroState) {
    LinearModel11 model(0.5, 1.0);
    model.step(1.0);
    model.step(1.0);
    ASSERT_NE(0.0, model.step(0.0));

    model.reset();
    EXPECT_DOUBLE_EQ(0.0, model.step(0.0));
}

// Полюс модели первого порядка равен коэффициенту a.
TEST(LinearModel11Test, PoleEqualsCoefficientA) {
    LinearModel11 model(-0.4, 1.0);
    const auto poles = model.poles();

    ASSERT_EQ(1u, poles.size());
    EXPECT_DOUBLE_EQ(-0.4, poles[0].real());
    EXPECT_DOUBLE_EQ(0.0, poles[0].imag());
}

// Импульсная реакция затухает по геометрической прогрессии.
TEST(LinearModel11Test, ImpulseResponseDecaysGeometrically) {
    LinearModel11 model(0.5, 1.0);

    double expected = 1.0;
    for (int i = 0; i < 5; ++i) {
        EXPECT_NEAR(expected, model.step(i == 0 ? 1.0 : 0.0), 1e-12);
        expected *= 0.5;
    }
}

// Аналитическое решение переходного процесса при ступенчатом воздействии.
TEST(LinearModel11Test, StepResponseMatchesClosedFormSolution) {
    const double a = 0.7;
    const double b = 1.5;
    const double u = 1.0;
    LinearModel11 model(a, b);

    double expected = 0.0;
    for (int i = 0; i < 8; ++i) {
        expected = b * u * (1.0 - std::pow(a, i + 1)) / (1.0 - a);
        EXPECT_NEAR(expected, model.step(u), 1e-12);
    }
}

// Установка начального состояния влияет на первый шаг.
TEST(LinearModel11Test, SetInitialStateIsUsedInFirstStep) {
    LinearModel11 model(0.5, 2.0);
    model.setInitialState({3.0});

    EXPECT_NEAR(0.5 * 3.0 + 2.0 * 1.0, model.step(1.0), 1e-12);
}

// Пустое начальное состояние трактуется как нулевое.
TEST(LinearModel11Test, EmptyInitialStateIsTreatedAsZero) {
    LinearModel11 model(0.5, 2.0);
    model.setInitialState({});

    EXPECT_DOUBLE_EQ(2.0, model.step(1.0));
}

// Имя и текстовое описание модели непустые и корректные.
TEST(LinearModel11Test, NameAndEquationAreInformative) {
    LinearModel11 model(0.5, 1.0);

    EXPECT_EQ("Model 1.1 (Linear 1st order)", model.name());
    EXPECT_NE(std::string::npos, model.equation().find("y(tau+1)"));
}
