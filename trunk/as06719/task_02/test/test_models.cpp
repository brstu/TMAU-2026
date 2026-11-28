#include <gtest/gtest.h>
#include "../src/models.h"
#include <stdexcept>

TEST(Model110Test, CalculatesCorrectValues) {
    Model110 model(0.5, 1.0, 0.5);

    std::vector<double> u = {1.0, 1.0, 1.0};
    auto y = model.calculate(u, 3);

    ASSERT_EQ(y.size(), 4);
    EXPECT_DOUBLE_EQ(y[0], 0.0);
    EXPECT_DOUBLE_EQ(y[1], 1.0);
    EXPECT_DOUBLE_EQ(y[2], 1.5);
    EXPECT_DOUBLE_EQ(y[3], 2.25);
}

TEST(Model22Test, SaturatesInput) {
    Model22 model(0.5, 1.0, -1.0, 1.0);

    std::vector<double> u = {2.0, -2.0};
    auto y = model.calculate(u, 2);

    ASSERT_EQ(y.size(), 3);
    EXPECT_DOUBLE_EQ(y[1], 1.0);
    EXPECT_DOUBLE_EQ(y[2], -0.5);
}

TEST(Model34Test, UsesEulerMethod) {
    Model34 model(1.0, 0.1);

    std::vector<double> u;
    auto y = model.calculate(u, 2);

    ASSERT_EQ(y.size(), 3);
    EXPECT_DOUBLE_EQ(y[0], 1.0);
    EXPECT_NEAR(y[1], 0.9, 1e-10);
    EXPECT_NEAR(y[2], 0.819, 1e-10);
}

// ---------------- Additional Model 1.10 tests ----------------

TEST(Model110Test, UsesDelayedInput) {
    Model110 model(0.0, 1.0, 10.0);

    std::vector<double> u = {1.0, 2.0, 3.0, 4.0};
    auto y = model.calculate(u, 4);

    ASSERT_EQ(y.size(), 5);
    EXPECT_DOUBLE_EQ(y[1], 1.0);
    EXPECT_DOUBLE_EQ(y[2], 2.0);
    EXPECT_DOUBLE_EQ(y[3], 13.0);
    EXPECT_DOUBLE_EQ(y[4], 24.0);
}

TEST(Model110Test, HandlesEmptyInput) {
    Model110 model(0.5, 1.0, 1.0);

    auto y = model.calculate({}, 3);

    ASSERT_EQ(y.size(), 4);
    for (double value : y) {
        EXPECT_DOUBLE_EQ(value, 0.0);
    }
}

TEST(Model110Test, HandlesShortInput) {
    Model110 model(0.0, 1.0, 0.0);

    auto y = model.calculate({2.0}, 3);

    ASSERT_EQ(y.size(), 4);
    EXPECT_DOUBLE_EQ(y[1], 2.0);
    EXPECT_DOUBLE_EQ(y[2], 0.0);
    EXPECT_DOUBLE_EQ(y[3], 0.0);
}

TEST(Model110Test, HandlesZeroSteps) {
    Model110 model(0.5, 1.0, 1.0);

    auto y = model.calculate({1.0}, 0);

    ASSERT_EQ(y.size(), 1);
    EXPECT_DOUBLE_EQ(y[0], 0.0);
}

TEST(Model110Test, RejectsNegativeSteps) {
    Model110 model(0.5, 1.0, 1.0);

    EXPECT_THROW(model.calculate({}, -1), std::invalid_argument);
}

// ---------------- Additional Model 2.2 tests ----------------

TEST(Model22Test, HandlesInputAtSaturationLimits) {
    Model22 model(0.0, 1.0, -1.0, 1.0);

    auto y = model.calculate({1.0, -1.0}, 2);

    EXPECT_DOUBLE_EQ(y[1], 1.0);
    EXPECT_DOUBLE_EQ(y[2], -1.0);
}

TEST(Model22Test, HandlesEmptyInput) {
    Model22 model(0.5, 1.0);

    auto y = model.calculate({}, 3);

    ASSERT_EQ(y.size(), 4);
    for (double value : y) {
        EXPECT_DOUBLE_EQ(value, 0.0);
    }
}

TEST(Model22Test, AllowsEqualSaturationLimits) {
    Model22 model(0.0, 1.0, 0.5, 0.5);

    auto y = model.calculate({-10.0, 0.0, 10.0}, 3);

    EXPECT_DOUBLE_EQ(y[1], 0.5);
    EXPECT_DOUBLE_EQ(y[2], 0.5);
    EXPECT_DOUBLE_EQ(y[3], 0.5);
}

TEST(Model22Test, RejectsInvalidSaturationLimits) {
    EXPECT_THROW(
        Model22(0.5, 1.0, 2.0, -2.0),
        std::invalid_argument
    );
}

TEST(Model22Test, RejectsNegativeSteps) {
    Model22 model(0.5, 1.0);

    EXPECT_THROW(model.calculate({}, -1), std::invalid_argument);
}

// ---------------- Additional Model 3.4 tests ----------------

TEST(Model34Test, HandlesZeroCoefficient) {
    Model34 model(0.0, 0.1);

    auto y = model.calculate({}, 3);

    ASSERT_EQ(y.size(), 4);
    for (double value : y) {
        EXPECT_DOUBLE_EQ(value, 1.0);
    }
}

TEST(Model34Test, HandlesZeroSteps) {
    Model34 model(1.0, 0.1);

    auto y = model.calculate({}, 0);

    ASSERT_EQ(y.size(), 1);
    EXPECT_DOUBLE_EQ(y[0], 1.0);
}

TEST(Model34Test, RejectsZeroTimeStep) {
    EXPECT_THROW(Model34(1.0, 0.0), std::invalid_argument);
}

TEST(Model34Test, RejectsNegativeTimeStep) {
    EXPECT_THROW(Model34(1.0, -0.1), std::invalid_argument);
}

TEST(Model34Test, RejectsNegativeSteps) {
    Model34 model(1.0, 0.1);

    EXPECT_THROW(model.calculate({}, -1), std::invalid_argument);
}