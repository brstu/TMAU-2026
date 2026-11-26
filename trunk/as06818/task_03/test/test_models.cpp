#include <gtest/gtest.h>

#include <cmath>
#include <vector>

#include "../src/diff_model.hpp"
#include "../src/linear_model.hpp"
#include "../src/nonlinear_model.hpp"

// Ручной расчёт модели 1.8 (a1=0.5, a2=0.2, b1=0.3, b2=0.1) при u = 1:
//   y1 = 0.5*0 + 0.2*0 + 0.3*1 + 0.1*0 = 0.3
//   y2 = 0.5*0.3 + 0.2*0 + 0.3*1 + 0.1*1 = 0.55
//   y3 = 0.5*0.55 + 0.2*0.3 + 0.3 + 0.1 = 0.735
TEST(LinearModel18Test, HandCalculatedSteps) {
    LinearModel18 model;
    EXPECT_NEAR(model.step(1.0), 0.3, 1e-12);
    EXPECT_NEAR(model.step(1.0), 0.55, 1e-12);
    EXPECT_NEAR(model.step(1.0), 0.735, 1e-12);
}

TEST(LinearModel18Test, ImpulseInput) {
    LinearModel18 model;
    EXPECT_NEAR(model.step(1.0), 0.3, 1e-12);
    EXPECT_NEAR(model.step(0.0), 0.25, 1e-12);
    EXPECT_NEAR(model.step(0.0), 0.185, 1e-12);
}

TEST(LinearModel18Test, ResetRestoresInitialState) {
    LinearModel18 model;
    model.step(1.0);
    model.step(1.0);
    model.reset();
    EXPECT_NEAR(model.step(1.0), 0.3, 1e-12);
}

TEST(LinearModel18Test, NonZeroStart) {
    LinearModel18 model(0.5, 0.2, 0.3, 0.1, 1.0);
    EXPECT_NEAR(model.get_y(), 1.0, 1e-12);
    // y1 = 0.5*1 + 0.2*0 + 0.3*1 + 0.1*0 = 0.8
    EXPECT_NEAR(model.step(1.0), 0.8, 1e-12);
}

TEST(LinearModel18Test, CoefficientAccessors) {
    LinearModel18 model(0.7, 0.1, 0.2, 0.05);
    EXPECT_DOUBLE_EQ(model.get_a1(), 0.7);
    EXPECT_DOUBLE_EQ(model.get_a2(), 0.1);
}

// При постоянном u = 1 устойчивая модель выходит на
// y_inf = (b1 + b2) / (1 - a1 - a2) = 0.4 / 0.3.
TEST(LinearModel18Test, ConvergesToSteadyState) {
    LinearModel18 model;
    for (int i = 0; i < 100; ++i) {
        model.step(1.0);
    }
    EXPECT_NEAR(model.get_y(), 0.4 / 0.3, 1e-9);
}

// Модель 2.2: y1 = 0.7*0 + 0.5*sat(1.0) = 0.25 (u выше предела насыщения).
TEST(NonlinearModel22Test, SaturationCutsInput) {
    NonlinearModel22 model;
    EXPECT_NEAR(model.step(1.0), 0.25, 1e-12);
}

TEST(NonlinearSatTest, SatStaticCharacteristic) {
    const NonlinearModel22 model;
    EXPECT_DOUBLE_EQ(model.sat(0.3), 0.3);
    EXPECT_DOUBLE_EQ(model.sat(0.7), 0.5);
    EXPECT_DOUBLE_EQ(model.sat(-0.7), -0.5);
}

TEST(NonlinearModel22Test, LinearZoneSteps) {
    NonlinearModel22 model;
    // y1 = 0.7*0 + 0.5*0.2 = 0.1
    EXPECT_NEAR(model.step(0.2), 0.1, 1e-12);
    // y2 = 0.7*0.1 + 0.5*0.2 = 0.17
    EXPECT_NEAR(model.step(0.2), 0.17, 1e-12);
}

TEST(NonlinearModel22Test, ResetRestoresInitialState) {
    NonlinearModel22 model(0.7, 0.5, -0.5, 0.5, 0.4);
    model.step(0.2);
    model.reset();
    EXPECT_NEAR(model.get_y(), 0.4, 1e-12);
}

// Модель 3.6 (a=-0.1, b=0.5, dt=0.1), Эйлер вручную:
//   y1 = 0 + 0.1*(0 + 0.5*1) = 0.05
//   y2 = 0.05 + 0.1*(-0.1*0.05^3 + 0.5) = 0.1 (с точностью до куба)
TEST(DiffModel36Test, EulerHandCalculated) {
    DiffModel36 model;
    EXPECT_NEAR(model.step(1.0), 0.05, 1e-12);
    const double y1 = 0.05;
    EXPECT_NEAR(model.step(1.0), y1 + 0.1 * (-0.1 * y1 * y1 * y1 + 0.5),
                1e-12);
}

// Отрицательный кубический член ограничивает рост при постоянном u:
// равновесие y^3 = b*u / (-a) = 5*u, при u = 0.2 -> y = 1.
TEST(DiffModel36Test, ConvergesToEquilibrium) {
    DiffModel36 model;
    for (int i = 0; i < 800; ++i) {
        model.step(0.2);
    }
    EXPECT_NEAR(model.get_y(), 1.0, 1e-3);
}

TEST(DiffModel36Test, ResetAndAccessors) {
    DiffModel36 model(-0.1, 0.5, 0.2, 0.5);
    model.step(1.0);
    model.reset();
    EXPECT_NEAR(model.get_y(), 0.5, 1e-12);
    EXPECT_DOUBLE_EQ(model.get_dt(), 0.2);
}
