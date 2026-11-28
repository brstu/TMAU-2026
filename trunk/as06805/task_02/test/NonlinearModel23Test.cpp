#include <gtest/gtest.h>

#include "NonlinearModel23.h"

namespace {

struct DeadZoneParams {
    double u;
    double expected;
};

class DeadZoneBoundaryTest : public ::testing::TestWithParam<DeadZoneParams> {};

// Значение зоны нечувствительности на граничных и внутренних точках.
TEST_P(DeadZoneBoundaryTest, DeadZoneReturnsExpectedValue) {
    const DeadZoneParams p = GetParam();
    NonlinearModel23 model(0.5, 1.0, 0.2);

    EXPECT_NEAR(p.expected, model.deadZone(p.u), 1e-12);
}

INSTANTIATE_TEST_SUITE_P(
    BoundaryValues,
    DeadZoneBoundaryTest,
    ::testing::Values(
        // Граничные точки +/- delta: зона нечувствительности равна нулю.
        DeadZoneParams{0.2, 0.0},
        DeadZoneParams{-0.2, 0.0},
        // Внутри зоны нечувствительности.
        DeadZoneParams{0.0, 0.0},
        DeadZoneParams{0.19, 0.0},
        DeadZoneParams{-0.19, 0.0},
        // За пределами зоны нечувствительности.
        DeadZoneParams{0.5, 0.3},
        DeadZoneParams{-0.5, -0.3},
        DeadZoneParams{1.2, 1.0}));

}  // namespace

// На границе зоны нечувствительности выход не изменяется.
TEST(NonlinearModel23Test, OutputIsZeroOnDeadZoneBoundary) {
    NonlinearModel23 model(0.5, 1.0, 0.2);

    EXPECT_DOUBLE_EQ(0.0, model.step(0.2));
    EXPECT_DOUBLE_EQ(0.0, model.step(-0.2));
}

// Переходный процесс при постоянном воздействии за зоной нечувствительности.
TEST(NonlinearModel23Test, StepResponseFollowsRecurrence) {
    NonlinearModel23 model(0.5, 2.0, 0.1);

    EXPECT_NEAR(2.0 * (1.0 - 0.1), model.step(1.0), 1e-12);
    EXPECT_NEAR(0.5 * (2.0 * 0.9) + 2.0 * 0.9, model.step(1.0), 1e-12);
}

// Инвариант: нулевой вход и нулевое состояние => покой.
TEST(NonlinearModel23Test, ZeroInputAndZeroStateStayAtRest) {
    NonlinearModel23 model(0.5, 1.0);

    for (int i = 0; i < 10; ++i) {
        EXPECT_DOUBLE_EQ(0.0, model.step(0.0));
    }
}

// Сброс состояния.
TEST(NonlinearModel23Test, ResetRestoresZeroState) {
    NonlinearModel23 model(0.5, 1.0);
    model.step(1.0);

    model.reset();
    EXPECT_DOUBLE_EQ(0.0, model.step(0.0));
}

// Начальное состояние учитывается в первом шаге.
TEST(NonlinearModel23Test, SetInitialStateIsUsedInFirstStep) {
    NonlinearModel23 model(0.5, 1.0, 0.1);
    model.setInitialState({2.0});

    EXPECT_NEAR(0.5 * 2.0 + 1.0 * (0.5 - 0.1), model.step(0.5), 1e-12);
}

// Полюс нелинейной модели равен коэффициенту a.
TEST(NonlinearModel23Test, PoleEqualsCoefficientA) {
    NonlinearModel23 model(0.4, 1.0);
    const auto poles = model.poles();

    ASSERT_EQ(1u, poles.size());
    EXPECT_DOUBLE_EQ(0.4, poles[0].real());
}

// Имя, описание и параметры модели.
TEST(NonlinearModel23Test, NameEquationAndParameters) {
    NonlinearModel23 model(0.4, 1.0, 0.3);

    EXPECT_EQ("Model 2.3 (Dead-zone non-linearity)", model.name());
    EXPECT_NE(std::string::npos, model.equation().find("delta"));
    EXPECT_DOUBLE_EQ(0.3, model.delta());
    EXPECT_DOUBLE_EQ(0.4, model.a());
    EXPECT_DOUBLE_EQ(1.0, model.b());
}
