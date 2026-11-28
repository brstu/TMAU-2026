#include <gtest/gtest.h>
#include "../src/models.h"

struct ModelTestCase {
    double a;
    double b;
    std::vector<double> input;
    std::vector<double> expected_output;
};

class LinearModelParamTest : public ::testing::TestWithParam<ModelTestCase> {};

TEST_P(LinearModelParamTest, CorrectOutputValues) {
    const auto& param = GetParam();
    LinearModel1_4 model(param.a, param.b);
    auto result = model.simulate(param.input);

    ASSERT_EQ(result.size(), param.expected_output.size());
    for (size_t i = 0; i < result.size(); ++i) {
        EXPECT_NEAR(result[i], param.expected_output[i], 1e-6);
    }
}

INSTANTIATE_TEST_SUITE_P(
    LinearModelCalculations,
    LinearModelParamTest,
    ::testing::Values(
        ModelTestCase{0.8, 0.2, {1.0, 1.0, 1.0}, {0.2, 0.16, 0.128}},
        ModelTestCase{0.5, 0.4, {1.0, 0.0, 0.0}, {0.4, -0.2, -0.1}},
        ModelTestCase{0.6, 0.3, {2.0, 2.0, 1.0}, {0.6, 0.36, -0.084}}
    )
);

TEST(ModelInvariantTest, ZeroInputZeroState) {
    LinearModel1_4 linModel(0.8, 0.2);
    NonlinearModel2_8 nonlinModel(0.5, 0.3, 0.1);
    DiffModel3_2 diffModel(0.5, 0.1);

    std::vector zero_input(5, 0.0);

    auto res_lin = linModel.simulate(zero_input);
    auto res_nonlin = nonlinModel.simulate(zero_input);
    auto res_diff = diffModel.simulate(zero_input);

    for (int i = 0; i < 5; ++i) {
        EXPECT_DOUBLE_EQ(res_lin[i], 0.0);
        EXPECT_DOUBLE_EQ(res_nonlin[i], 0.0);
        EXPECT_DOUBLE_EQ(res_diff[i], 0.0);
    }
}

TEST(DiffModelTest, EulerAnalyticalComparison) {
    double b = 0.5;
    double dt = 0.1;
    DiffModel3_2 model(b, dt);

    std::vector u(10, 2.0);
    auto result = model.simulate(u);

    for (size_t t = 0; t < result.size(); ++t) {
        double expected = (static_cast<double>(t) + 1.0) * 0.1;
        EXPECT_NEAR(result[t], expected, 1e-7);
    }
}

TEST(NonlinearModelTest, StepByStepCalculation) {
    NonlinearModel2_8 model(0.5, 0.1, 0.2);

    double y1 = model.calculateNext(1.0);
    double expected_y1 = 0.2 + std::sin(1.0);
    EXPECT_NEAR(y1, expected_y1, 1e-6);

    double y2 = model.calculateNext(0.5);
    double expected_y2 = 0.5 * y1 + 0.2 * 0.5 + std::sin(0.5);
    EXPECT_NEAR(y2, expected_y2, 1e-6);

    double y3 = model.calculateNext(0.0);
    double expected_y3 = 0.5 * y2 - 0.1 * (y1 * y1);
    EXPECT_NEAR(y3, expected_y3, 1e-6);
}

TEST(NonlinearModelTest, ResetRestoresInitialState) {
    NonlinearModel2_8 model(0.5, 0.1, 0.2);
    double first = model.calculateNext(1.0);
    model.calculateNext(2.0);

    model.reset();
    EXPECT_DOUBLE_EQ(model.calculateNext(1.0), first);
}

TEST(LinearModelTest, ResetAndGetters) {
    LinearModel1_4 model(0.75, 0.25);
    EXPECT_DOUBLE_EQ(model.getA(), 0.75);
    EXPECT_DOUBLE_EQ(model.getB(), 0.25);

    double first = model.calculateNext(4.0);
    model.calculateNext(10.0);

    model.reset();
    EXPECT_DOUBLE_EQ(model.calculateNext(4.0), first);
}

TEST(DiffModelTest, ResetRestoresZeroState) {
    DiffModel3_2 model(1.5, 0.2);
    double first = model.calculateNext(2.0);
    model.calculateNext(2.0);

    model.reset();
    EXPECT_DOUBLE_EQ(model.calculateNext(2.0), first);
}

TEST(ModelEdgeCaseTest, EmptyInputReturnsEmptyOutput) {
    LinearModel1_4 lin(0.5, 0.5);
    NonlinearModel2_8 nonlin(0.5, 0.1, 0.2);
    DiffModel3_2 diff(1.0, 0.1);

    std::vector<double> empty;
    EXPECT_TRUE(lin.simulate(empty).empty());
    EXPECT_TRUE(nonlin.simulate(empty).empty());
    EXPECT_TRUE(diff.simulate(empty).empty());
}

TEST(ModelEdgeCaseTest, NegativeInputValues) {
    LinearModel1_4 model(0.5, 0.5);
    std::vector inputs{-2.0, -2.0};
    auto outputs = model.simulate(inputs);

    ASSERT_EQ(outputs.size(), 2);
    EXPECT_DOUBLE_EQ(outputs[0], -1.0);
    EXPECT_DOUBLE_EQ(outputs[1], -0.5);
}