#include <gtest/gtest.h>

#include <cmath>

#include "models.hpp"



TEST(Models, LinearStepResponseHandCalculated) {

    // a1 = 0.5, a2 = 0.2, a3 = 0.1, b = 1, u = 1
    // y1 = 1; y2 = 0.5*1 + 1 = 1.5; y3 = 0.5*1.5 + 0.2*1 + 1 = 1.95;
    // y4 = 0.5*1.95 + 0.2*1.5 + 0.1*1 + 1 = 2.375

    LinearModel16 model(0.5, 0.2, 0.1, 1.0);

    EXPECT_NEAR(model.step(1.0), 1.0, 1e-12);
    EXPECT_NEAR(model.step(1.0), 1.5, 1e-12);
    EXPECT_NEAR(model.step(1.0), 1.95, 1e-12);
    EXPECT_NEAR(model.step(1.0), 2.375, 1e-12);
}

TEST(Models, NonlinearStepResponseHandCalculated) {
 
    // a = 0.8, b = 0.5, u = 1: y1 = 0.5; y2 = 0.8*tanh(0.5) + 0.5, tanh(0.5) = 0.46211715726
 
    NonlinearModel210 model(0.8, 0.5);

    EXPECT_NEAR(model.step(1.0), 0.5, 1e-12);
    EXPECT_NEAR(model.step(1.0), 0.8 * 0.46211715726 + 0.5, 1e-9);
}

TEST(Models, DiffModelEulerStepsHandCalculated) {
   
    // a = 0.5, dt = 0.1, y0 = 1: y1 = 1 - 0.05*1 = 0.95; y2 = 0.95 - 0.05*0.9025 = 0.904875
   
    DiffModel34 model(0.5, 0.1, 1.0);

    EXPECT_NEAR(model.step(0.0), 0.95, 1e-12);
    EXPECT_NEAR(model.step(0.0), 0.904875, 1e-12);
}



TEST(Models, ZeroInputAndZeroStateIsRest) {
    LinearModel16 linear(0.5, 0.2, 0.1, 1.0);
    NonlinearModel210 nonlinear(0.8, 0.5);
    DiffModel34 diff(0.5, 0.1, 0.0);

    for (int i = 0; i < 20; ++i) {
        EXPECT_DOUBLE_EQ(linear.step(0.0), 0.0);
        EXPECT_DOUBLE_EQ(nonlinear.step(0.0), 0.0);
        EXPECT_DOUBLE_EQ(diff.step(0.0), 0.0);
    }
}



TEST(Models, NonlinearFeedbackIsBoundedByA) {
    NonlinearModel210 model(0.8, 1.0);
    model.step(10.0);   // huge state (1000)

    for (int i = 0; i < 20; ++i) {
        EXPECT_LE(std::abs(model.step(0.0)), 0.8);
    }
}



class EulerAccuracy : public ::testing::TestWithParam<double> {};

TEST_P(EulerAccuracy, ErrorIsSmallForSmallStep) {
    const double dt = GetParam();
    const int steps = static_cast<int>(std::lround(2.0 / dt));   // t = 2
    DiffModel34 model(0.5, dt, 1.0);

    for (int i = 0; i < steps; ++i) {
        model.step(0.0);
    }

    EXPECT_NEAR(model.output(), diffModel34Exact(0.5, 1.0, 2.0), 0.5 * dt);
}

INSTANTIATE_TEST_SUITE_P(Steps, EulerAccuracy, ::testing::Values(0.1, 0.01, 0.001));