#include "DifferentialModel31.h"
#include "gtest/gtest.h"

#include <cmath>
#include <stdexcept>

TEST(DifferentialModelTests, PerformsOneEulerStepCorrectly) {
    DifferentialModel31 model(0.4, 1.0);
    EXPECT_NEAR(model.step(123.0, 0.1), 0.96, 1e-12);
}

TEST(DifferentialModelTests, ZeroInitialStateRemainsZero) {
    DifferentialModel31 model(0.4, 0.0);
    EXPECT_DOUBLE_EQ(model.step(1.0, 0.1), 0.0);
    EXPECT_DOUBLE_EQ(model.step(-1.0, 0.1), 0.0);
}

TEST(DifferentialModelTests, EulerStepApproachesAnalyticalSolution) {
    const double a = 0.4;
    const double dt = 0.001;
    DifferentialModel31 model(a, 1.0);
    const double numerical = model.step(0.0, dt);
    const double analytical = std::exp(-a * dt);
    EXPECT_NEAR(numerical, analytical, 1e-6);
}

TEST(DifferentialModelTests, RejectsNonPositiveIntegrationStep) {
    DifferentialModel31 model(0.4, 1.0);
    EXPECT_THROW(model.step(0.0, 0.0), std::invalid_argument);
}
