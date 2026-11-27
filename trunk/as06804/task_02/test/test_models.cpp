#include <gtest/gtest.h>

#include <cmath>

#include "DiffEqModel.h"
#include "LinearModel.h"
#include "NonLinearModel.h"

// ---------------------------------------------------------------------------
// Model 1.4: y[k+1] = a*y[k] + b*(u[k] - u[k-1])
// ---------------------------------------------------------------------------

TEST(LinearModelTest, FirstStepTreatsPreviousInputAsCurrent) {
    LinearModel model(0.5, 2.0, 1.0, 0.1);
    model.step(3.0);
    EXPECT_DOUBLE_EQ(model.getY(), 0.5);
}

TEST(LinearModelTest, FollowsRecurrenceForChangingInput) {
    LinearModel model(0.5, 2.0, 1.0, 0.1);
    model.step(0.0); // u_prev = 0 -> y = 0.5
    model.step(1.0); // y = 0.5*0.5 + 2*(1 - 0) = 2.25
    model.step(1.0); // y = 0.5*2.25 + 2*(1 - 1) = 1.125

    EXPECT_DOUBLE_EQ(model.getY(), 1.125);
}

TEST(LinearModelTest, ZeroInputDecaysWhenAbsAIsLessThanOne) {
    LinearModel model(0.5, 1.0, 1.0, 0.1);
    for (int i = 0; i < 10; ++i) {
        model.step(0.0);
    }
    EXPECT_NEAR(model.getY(), std::pow(0.5, 10), 1e-12);
}

TEST(LinearModelTest, ImpulseProducesInputDifferenceResponse) {
    LinearModel model(0.5, 1.0, 0.0, 0.1);
    model.step(1.0); // y = 0
    EXPECT_DOUBLE_EQ(model.getY(), 0.0);
    model.step(0.0); // y = 0.5*0 + 1*(0 - 1) = -1
    EXPECT_DOUBLE_EQ(model.getY(), -1.0);
    model.step(0.0); // y = 0.5*(-1) + 1*(0 - 0) = -0.5
    EXPECT_DOUBLE_EQ(model.getY(), -0.5);
}

// ---------------------------------------------------------------------------
// Model 2.6: y[k+1] = a1*y[k]^3 - a2*y[k-1] + b*u[k]^2
// ---------------------------------------------------------------------------

TEST(NonLinearModelTest, FirstStepUsesInitialStateAsPrevious) {
    NonLinearModel model(0.5, 0.2, 1.0, 1.0, 0.1);
    model.step(0.0);
    EXPECT_NEAR(model.getY(), 0.3, 1e-12); // 0.5*1^3 - 0.2*1
}

TEST(NonLinearModelTest, SecondStepKnownValue) {
    NonLinearModel model(0.5, 0.2, 1.0, 1.0, 0.1);
    model.step(0.0); // 0.3
    model.step(0.0); // 0.5*0.3^3 - 0.2*1 = -0.1865
    EXPECT_NEAR(model.getY(), -0.1865, 1e-12);
}

TEST(NonLinearModelTest, ZeroStateAndZeroInputStayAtRest) {
    NonLinearModel model(0.7, 0.3, 2.0, 0.0, 0.1);
    for (int i = 0; i < 5; ++i) {
        model.step(0.0);
        EXPECT_DOUBLE_EQ(model.getY(), 0.0);
    }
}

TEST(NonLinearModelTest, InputIsSquared) {
    NonLinearModel model(0.0, 0.0, 2.0, 0.0, 0.1);
    model.step(-3.0);
    EXPECT_DOUBLE_EQ(model.getY(), 18.0);
}

// ---------------------------------------------------------------------------
// Model 3.8: dy/dt = -a1*y - a2*y^2 + b*u, explicit Euler
// ---------------------------------------------------------------------------

TEST(DiffEqModelTest, OneEulerStepIsCalculatedManually) {
    DiffEqModel model(1.0, 0.5, 2.0, 1.0, 0.1);
    model.step(3.0);
    // dydt = -1 - 0.5 + 6 = 4.5; y = 1 + 0.1*4.5 = 1.45
    EXPECT_NEAR(model.getY(), 1.45, 1e-12);
}

TEST(DiffEqModelTest, ZeroStateAndZeroInputStayAtRest) {
    DiffEqModel model(1.0, 0.5, 2.0, 0.0, 0.1);
    model.step(0.0);
    EXPECT_DOUBLE_EQ(model.getY(), 0.0);
}

TEST(DiffEqModelTest, BiasTermShiftsState) {
    DiffEqModel model(0.0, 0.0, 1.0, 0.0, 0.1);
    model.step(1.0);
    EXPECT_NEAR(model.getY(), 0.1, 1e-12);
}

TEST(DiffEqModelTest, EulerApproachesAnalyticSolution) {
    const double dt = 0.01;
    DiffEqModel model(0.5, 0.0, 1.0, 0.0, dt);
    for (int i = 0; i < 100; ++i) {
        model.step(1.0);
    }

    // dy/dt = -0.5y + 1, y(0) = 0 -> y(1) = 2*(1 - exp(-0.5)).
    const double exact = 2.0 * (1.0 - std::exp(-0.5));
    EXPECT_NEAR(model.getY(), exact, 5e-3);
}
