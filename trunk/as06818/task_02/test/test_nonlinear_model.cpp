#include <gtest/gtest.h>

#include <cmath>
#include <vector>

#include "../src/input_signal.hpp"
#include "../src/nonlinear_model.hpp"
#include "../src/simulator.hpp"

// Ручной расчёт для коэффициентов по умолчанию (a=0.7, b=0.5,
// Umin=-0.5, Umax=0.5) при ступенчатом входе u = 1:
//   y1 = 0.7*0 + 0.5*sat(1) = 0.5*0.5 = 0.25
//   y2 = 0.7*0.25 + 0.25 = 0.425
TEST(NonlinearModel22Test, StepInputHandCalculated) {
    NonlinearModel22 model;
    EXPECT_NEAR(model.step(1.0), 0.25, 1e-12);
    EXPECT_NEAR(model.step(1.0), 0.425, 1e-12);
}

// Инвариант покоя: при u = 0 и y(0) = 0 система остаётся в покое.
TEST(NonlinearModel22Test, RestInvariant) {
    NonlinearModel22 model;
    for (int tau = 0; tau < 10; ++tau) {
        EXPECT_DOUBLE_EQ(model.step(0.0), 0.0);
    }
}

// reset возвращает модель к начальному состоянию.
TEST(NonlinearModel22Test, ResetRestoresState) {
    NonlinearModel22 model;
    model.step(1.0);
    model.reset();
    EXPECT_DOUBLE_EQ(model.get_y(), 0.0);
}

// Параметризованный тест граничных значений нелинейности насыщения:
// u ровно на границах, внутри зоны и за границами.
struct SaturationParams {
    double u;
    double expected_sat;
};

class SaturationBoundarySuite
    : public testing::TestWithParam<SaturationParams> {
};

TEST_P(SaturationBoundarySuite, SatClampsToLimits) {
    const SaturationParams& p = GetParam();
    NonlinearModel22 model;
    EXPECT_NEAR(model.sat(p.u), p.expected_sat, 1e-12);
}

INSTANTIATE_TEST_SUITE_P(
    BoundaryValues, SaturationBoundarySuite,
    testing::Values(
        SaturationParams{0.5, 0.5},
        SaturationParams{-0.5, -0.5},
        SaturationParams{0.7, 0.5},
        SaturationParams{-0.9, -0.5},
        SaturationParams{0.25, 0.25},
        SaturationParams{0.0, 0.0}));

// Насыщение видно и через шаг модели: при a = 0, b = 1 выход шага
// совпадает со статической характеристикой sat(u).
TEST(NonlinearModel22Test, StepRevealsSaturation) {
    NonlinearModel22 probe(0.0, 1.0);
    probe.step(0.7);
    EXPECT_NEAR(probe.get_y(), 0.5, 1e-12);
}

// Гармоническое воздействие не выводит устойчивую модель за пределы
// ограниченной области (инвариант физического смысла).
TEST(NonlinearModel22Test, HarmonicResponseBounded) {
    NonlinearModel22 model;
    const std::vector<SimulationPoint> points =
        simulate(model, SignalType::Harmonic, 500);

    for (const SimulationPoint& point : points) {
        EXPECT_LT(std::fabs(point.y), 1.7);
    }
}
