#include <gtest/gtest.h>

#include <array>
#include <cmath>
#include <vector>

#include "../src/input_signal.hpp"
#include "../src/linear_model.hpp"
#include "../src/simulator.hpp"

// Ручной расчёт для коэффициентов по умолчанию (a1=0.5, a2=0.2, b1=0.3,
// b2=0.1) при ступенчатом входе u = 1:
//   y1 = 0.5*0 + 0.2*0 + 0.3*1 + 0.1*0 = 0.3
//   y2 = 0.5*0.3 + 0.2*0 + 0.3*1 + 0.1*1 = 0.55
//   y3 = 0.5*0.55 + 0.2*0.3 + 0.3 + 0.1 = 0.735
TEST(LinearModel18Test, StepInputHandCalculated) {
    LinearModel18 model;
    EXPECT_NEAR(model.step(1.0), 0.3, 1e-12);
    EXPECT_NEAR(model.step(1.0), 0.55, 1e-12);
    EXPECT_NEAR(model.step(1.0), 0.735, 1e-12);
}

// Импульсный вход: u = [1, 0, 0, ...].
TEST(LinearModel18Test, ImpulseInputHandCalculated) {
    LinearModel18 model;
    EXPECT_NEAR(model.step(1.0), 0.3, 1e-12);
    EXPECT_NEAR(model.step(0.0), 0.25, 1e-12);
    EXPECT_NEAR(model.step(0.0), 0.185, 1e-12);
}

// reset возвращает модель к начальному состоянию.
TEST(LinearModel18Test, ResetRestoresState) {
    LinearModel18 model(0.5, 0.2, 0.3, 0.1);
    model.step(1.0);
    model.step(1.0);
    model.reset();
    EXPECT_NEAR(model.step(1.0), 0.3, 1e-12);
}

// Ненулевое начальное условие y(0) = y_start:
//   y1 = 0.5*1 + 0.2*0 + 0.3*1 + 0.1*0 = 0.8
TEST(LinearModel18Test, CustomInitialState) {
    LinearModel18 model(0.5, 0.2, 0.3, 0.1, 1.0);
    EXPECT_NEAR(model.step(1.0), 0.8, 1e-12);
}

// Геттеры коэффициентов и текущего состояния.
TEST(LinearModel18Test, CoefficientsAccessor) {
    LinearModel18 model(0.7, 0.1, 0.2, 0.05);
    EXPECT_DOUBLE_EQ(model.get_a1(), 0.7);
    EXPECT_DOUBLE_EQ(model.get_a2(), 0.1);
    EXPECT_DOUBLE_EQ(model.get_y(), 0.0);

    model.step(1.0);
    EXPECT_DOUBLE_EQ(model.get_y(), 0.2);
}

// Параметризованный тест: точные значения первых шагов при разных
// наборах коэффициентов (u = 1).
struct LinearStepParams {
    double a1;
    double a2;
    double b1;
    double b2;
    double expected_y1;
    double expected_y2;
};

class LinearModelStepSuite : public testing::TestWithParam<LinearStepParams> {
};

TEST_P(LinearModelStepSuite, FirstStepsMatchHandCalculation) {
    const LinearStepParams& p = GetParam();
    LinearModel18 model(p.a1, p.a2, p.b1, p.b2);
    EXPECT_NEAR(model.step(1.0), p.expected_y1, 1e-12);
    EXPECT_NEAR(model.step(1.0), p.expected_y2, 1e-12);
}

INSTANTIATE_TEST_SUITE_P(
    HandCalculatedSets, LinearModelStepSuite,
    testing::Values(
        LinearStepParams{0.5, 0.2, 0.3, 0.1, 0.3, 0.55},
        LinearStepParams{0.9, 0.05, 0.2, 0.0, 0.2, 0.38},
        LinearStepParams{0.0, 0.5, 0.5, 0.5, 0.5, 1.0},
        LinearStepParams{0.3, 0.3, 0.1, 0.1, 0.1, 0.23}));

// Параметризованный тест сходимости: для устойчивых наборов коэффициентов
// процесс сходится — приращение на длинном горизонте стремится к нулю.
class LinearModelConvergenceSuite
    : public testing::TestWithParam<std::array<double, 4>> {
};

TEST_P(LinearModelConvergenceSuite, ProcessConverges) {
    const std::array<double, 4>& p = GetParam();
    LinearModel18 model(p[0], p[1], p[2], p[3]);

    // Горизонт 500 шагов: у набора {0.9, 0.05} доминирующий полюс
    // z1 = 0.952, и процесс сходится медленно.
    const std::vector<SimulationPoint> points =
        simulate(model, SignalType::Step, 500);

    ASSERT_EQ(points.size(), 500U);
    const double delta =
        std::fabs(points[499].y - points[498].y);
    EXPECT_LT(delta, 1e-6);
    EXPECT_LT(std::fabs(points[499].y), 10.0);
}

INSTANTIATE_TEST_SUITE_P(
    StableCoefficientSets, LinearModelConvergenceSuite,
    testing::Values(
        std::array<double, 4>{0.5, 0.2, 0.3, 0.1},
        std::array<double, 4>{0.9, 0.05, 0.2, 0.0},
        std::array<double, 4>{0.3, 0.3, 0.1, 0.1},
        std::array<double, 4>{0.8, 0.0, 0.5, 0.0}));
