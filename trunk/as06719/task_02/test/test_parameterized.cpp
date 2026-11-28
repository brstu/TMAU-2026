#include <gtest/gtest.h>
#include "../src/models.h"

class Model110ParameterizedTest
    : public testing::TestWithParam<double> {
};

TEST_P(Model110ParameterizedTest, CalculatesFirstStep) {
    double a1 = GetParam();

    Model110 model(a1, 1.0, 0.0);
    std::vector<double> u = {1.0};

    auto y = model.calculate(u, 1);

    ASSERT_EQ(y.size(), 2);
    EXPECT_DOUBLE_EQ(y[1], 1.0);
}

INSTANTIATE_TEST_SUITE_P(
    DifferentCoefficients,
    Model110ParameterizedTest,
    testing::Values(0.2, 0.5, 0.8, -0.5)
);
