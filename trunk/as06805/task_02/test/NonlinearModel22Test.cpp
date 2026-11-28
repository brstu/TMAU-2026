#include <gtest/gtest.h>

#include "NonlinearModel22.h"

namespace {

struct SaturationParams {
    double u;
    double expected;
};

class SaturationBoundaryTest : public ::testing::TestWithParam<SaturationParams> {};

// Функция насыщения на граничных и внутренних точках.
TEST_P(SaturationBoundaryTest, SaturateReturnsExpectedValue) {
    const SaturationParams p = GetParam();
    NonlinearModel22 model(0.5, 1.0, -1.0, 1.0);

    EXPECT_NEAR(p.expected, model.saturate(p.u), 1e-12);
}

INSTANTIATE_TEST_SUITE_P(
    BoundaryValues,
    SaturationBoundaryTest,
    ::testing::Values(
        // Граничные точки Umin и Umax.
        SaturationParams{1.0, 1.0},
        SaturationParams{-1.0, -1.0},
        // Точки за границами насыщения.
        SaturationParams{2.5, 1.0},
        SaturationParams{-2.5, -1.0},
        // Внутри линейного участка.
        SaturationParams{0.0, 0.0},
        SaturationParams{0.35, 0.35},
        SaturationParams{-0.75, -0.75}));

}  // namespace

// Граничная точка Umax не насыщается (u = Umax остаётся u = Umax).
TEST(NonlinearModel22Test, SaturationAtUpperBoundIsIdentity) {
    NonlinearModel22 model(0.5, 1.0, -1.0, 1.0);

    EXPECT_DOUBLE_EQ(1.0, model.saturate(1.0));
    // Значение за границей ограничивается уровнем Umax.
    EXPECT_DOUBLE_EQ(1.0, model.saturate(1.0001));
    EXPECT_DOUBLE_EQ(1.0, model.saturate(100.0));
}

// Граничная точка Umin не насыщается.
TEST(NonlinearModel22Test, SaturationAtLowerBoundIsIdentity) {
    NonlinearModel22 model(0.5, 1.0, -1.0, 1.0);

    EXPECT_DOUBLE_EQ(-1.0, model.saturate(-1.0));
    // Значение за границей ограничивается уровнем Umin.
    EXPECT_DOUBLE_EQ(-1.0, model.saturate(-1.0001));
    EXPECT_DOUBLE_EQ(-1.0, model.saturate(-100.0));
}

// Переходный процесс при постоянном входном воздействии.
TEST(NonlinearModel22Test, StepResponseFollowsRecurrence) {
    NonlinearModel22 model(0.5, 2.0, -1.0, 1.0);

    // sat(0.5) = 0.5 => y(1) = 0.5*0 + 2.0*0.5 = 1.0
    EXPECT_NEAR(2.0 * 0.5, model.step(0.5), 1e-12);
    // y(2) = 0.5*y(1) + 2.0*0.5 = 1.5
    EXPECT_NEAR(0.5 * 1.0 + 2.0 * 0.5, model.step(0.5), 1e-12);
}

// Насыщение ограничивает входной сигнал и, следовательно, выход.
TEST(NonlinearModel22Test, SaturatedInputLimitsOutput) {
    NonlinearModel22 model(0.0, 1.0, -1.0, 1.0);

    EXPECT_DOUBLE_EQ(1.0, model.step(10.0));
    EXPECT_DOUBLE_EQ(-1.0, model.step(-10.0));
}

// Инвариант: нулевой вход и нулевое состояние => покой.
TEST(NonlinearModel22Test, ZeroInputAndZeroStateStayAtRest) {
    NonlinearModel22 model(0.5, 1.0);

    for (int i = 0; i < 10; ++i) {
        EXPECT_DOUBLE_EQ(0.0, model.step(0.0));
    }
}

// Сброс состояния.
TEST(NonlinearModel22Test, ResetRestoresZeroState) {
    NonlinearModel22 model(0.5, 1.0);
    model.step(1.0);

    model.reset();
    EXPECT_DOUBLE_EQ(0.0, model.step(0.0));
}

// Начальное состояние учитывается в первом шаге.
TEST(NonlinearModel22Test, SetInitialStateIsUsedInFirstStep) {
    NonlinearModel22 model(0.5, 1.0);
    model.setInitialState({2.0});

    EXPECT_NEAR(0.5 * 2.0 + 1.0 * 0.5, model.step(0.5), 1e-12);
}

// Полюс нелинейной модели равен коэффициенту a.
TEST(NonlinearModel22Test, PoleEqualsCoefficientA) {
    NonlinearModel22 model(0.6, 1.0);
    const auto poles = model.poles();

    ASSERT_EQ(1u, poles.size());
    EXPECT_DOUBLE_EQ(0.6, poles[0].real());
}

// Имя и описание модели, параметры насыщения доступны извне.
TEST(NonlinearModel22Test, NameEquationAndParameters) {
    NonlinearModel22 model(0.6, 0.4, -2.0, 2.0);

    EXPECT_EQ("Model 2.2 (Actuator saturation)", model.name());
    EXPECT_NE(std::string::npos, model.equation().find("Umax"));
    EXPECT_DOUBLE_EQ(-2.0, model.uMin());
    EXPECT_DOUBLE_EQ(2.0, model.uMax());
    EXPECT_DOUBLE_EQ(0.6, model.a());
    EXPECT_DOUBLE_EQ(0.4, model.b());
}
