#include <gtest/gtest.h>

#include <cmath>
#include <vector>

#include "../src/diff_model.hpp"
#include "../src/input_signal.hpp"
#include "../src/simulator.hpp"

// Ручной расчёт первого шага Эйлера для коэффициентов по умолчанию
// (a=-0.1, b=0.5, dt=0.1) при y(0)=0, u=1:
//   y1 = 0 + 0.1 * (-0.1*0 + 0.5*1) = 0.05
TEST(DiffModel36Test, EulerStepHandCalculated) {
    DiffModel36 model;
    EXPECT_NEAR(model.step(1.0), 0.05, 1e-12);
}

// Ненулевое начальное условие и нулевой вход:
//   y1 = 1 + 0.1 * (-0.1 * 1^3) = 1 - 0.01 = 0.99
TEST(DiffModel36Test, CustomInitialState) {
    DiffModel36 model(-0.1, 0.5, 0.1, 1.0);
    EXPECT_NEAR(model.step(0.0), 0.99, 1e-12);
}

// Инвариант покоя: u = 0, y(0) = 0 => y остаётся нулевым.
TEST(DiffModel36Test, RestInvariant) {
    DiffModel36 model;
    for (int tau = 0; tau < 10; ++tau) {
        EXPECT_DOUBLE_EQ(model.step(0.0), 0.0);
    }
}

// reset возвращает модель к начальному состоянию.
TEST(DiffModel36Test, ResetRestoresState) {
    DiffModel36 model(-0.1, 0.5, 0.1, 1.0);
    model.step(0.0);
    model.reset();
    EXPECT_DOUBLE_EQ(model.get_y(), 1.0);
}

// Аналитическое решение при u = 0: dy/dt = a*y^3 разделяется,
//   y(t) = y0 / sqrt(1 - 2*a*y0^2*t),  y0 = 1, a = -0.1:
//   y(2) = 1 / sqrt(1.4) = 0.845154...
// Метод Эйлера первого порядка: при dt = 0.01 ошибка < 1e-3.
TEST(DiffModel36Test, EulerMatchesAnalyticalSolution) {
    const double a = -0.1;
    const double dt = 0.01;
    DiffModel36 model(a, 0.5, dt, 1.0);

    for (int tau = 0; tau < 200; ++tau) {
        model.step(0.0);
    }

    const double exact = 1.0 / std::sqrt(1.0 - 2.0 * a * 1.0 * 2.0);
    EXPECT_NEAR(model.get_y(), exact, 1e-3);
}

// Параметризованный тест порядка точности: ошибка Эйлера убывает
// при измельчении шага dt (метод первого порядка, ошибка ~ O(dt)).
class EulerRefinementSuite : public testing::TestWithParam<double> {
};

TEST_P(EulerRefinementSuite, ErrorDecreasesWithSmallerDt) {
    const double a = -0.1;
    const double dt = GetParam();
    DiffModel36 model(a, 0.5, dt, 1.0);

    const int steps = static_cast<int>(2.0 / dt);  // интегрируем до t = 2
    for (int tau = 0; tau < steps; ++tau) {
        model.step(0.0);
    }

    const double exact = 1.0 / std::sqrt(1.0 - 2.0 * a * 2.0);
    const double error = std::fabs(model.get_y() - exact);
    EXPECT_LT(error, dt);  // ошибка убывает быстрее самого шага
}

INSTANTIATE_TEST_SUITE_P(
    TimeSteps, EulerRefinementSuite,
    testing::Values(0.1, 0.05, 0.01));
