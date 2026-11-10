#include <gtest/gtest.h>
#include "../src/models.h"

// Структура тестового набора данных
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
        // Тест 1: Ступенчатое воздействие [1.0, 1.0, 1.0] при a=0.8, b=0.2
        // t=0: y=0.8*0 + 0.2*(1-0) = 0.2
        // t=1: y=0.8*0.2 + 0.2*(1-1) = 0.16
        // t=2: y=0.8*0.16 + 0.2*(1-1) = 0.128
        ModelTestCase{0.8, 0.2, {1.0, 1.0, 1.0}, {0.2, 0.16, 0.128}},
        // Тест 2: Импульсное воздействие [1.0, 0.0, 0.0] при a=0.5, b=0.4
        // t=0: y=0.5*0 + 0.4*(1-0) = 0.4
        // t=1: y=0.5*0.4 + 0.4*(0-1) = 0.2 - 0.4 = -0.2
        // t=2: y=0.5*(-0.2) + 0.4*(0-0) = -0.1
        ModelTestCase{0.5, 0.4, {1.0, 0.0, 0.0}, {0.4, -0.2, -0.1}}
    )
);

// --- Тесты физических инвариантов ---

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

// Проверка интегратора Эйлера (Model 3.2) по сравнению с аналитическим решением y(t) = b * u * t
TEST(DiffModelTest, EulerAnalyticalComparison) {
    double b = 0.5;
    double dt = 0.1;
    DiffModel3_2 model(b, dt);

    std::vector u(10, 2.0); // u = 2.0
    auto result = model.simulate(u);

    // Аналитическое решение: y_next = y_prev + dt * b * u = y_prev + 0.1 * 0.5 * 2.0 = y_prev + 0.1
    for (size_t t = 0; t < result.size(); ++t) {
        double expected = (static_cast<double>(t) + 1.0) * 0.1;
        EXPECT_NEAR(result[t], expected, 1e-7);
    }
}