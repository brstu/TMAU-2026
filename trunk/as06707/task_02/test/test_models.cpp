#include <gtest/gtest.h>
#include "Model1_1.h"
#include "Model2_5.h"
#include "Model3_9.h"

// Тесты для Model 1.1
TEST(ModelTests, Model1_1_CorrectStep) {
    Model1_1 model(0.5, 2.0, 0.0); 
    EXPECT_DOUBLE_EQ(model.step(1.0), 2.0); 
    EXPECT_DOUBLE_EQ(model.step(1.0), 3.0); 
}

TEST(ModelTests, Model1_1_ResetAndGetY) {
    Model1_1 model(0.5, 2.0, 0.0); 
    model.step(1.0);
    EXPECT_DOUBLE_EQ(model.getY(), 2.0); 
    model.reset(10.0);
    EXPECT_DOUBLE_EQ(model.getY(), 10.0); 
}

// Тесты для Model 2.5
TEST(ModelTests, Model2_5_CorrectStep) {
    Model2_5 model(0.5, 2.0, 0.0);
    EXPECT_NEAR(model.step(1.0), 1.264241, 1e-5);
    EXPECT_NEAR(model.getY(), 1.264241, 1e-5);
    model.reset(-5.0);
    EXPECT_DOUBLE_EQ(model.getY(), -5.0);
}

TEST(ModelTests, Model2_5_NegativeInput) {
    Model2_5 model(0.5, 2.0, 0.0);
    // Этот тест покрывает ветку else if (u_tau < 0.0) для 100% покрытия
    EXPECT_NEAR(model.step(-1.0), -1.264241, 1e-5);
}

// Тесты для Model 3.9
TEST(ModelTests, Model3_9_CorrectStep) {
    Model3_9 model(2.0, 0.0);
    EXPECT_NEAR(model.step(1.0), 1.523188, 1e-5);
    EXPECT_NEAR(model.getY(), 1.523188, 1e-5);
    model.reset(5.0);
    EXPECT_DOUBLE_EQ(model.getY(), 5.0);
}

// Инварианты
TEST(ModelTests, ZeroInputZeroState) {
    Model1_1 model1(0.5, 1.0, 0.0);
    Model2_5 model2(0.5, 1.0, 0.0);
    Model3_9 model3(1.0, 0.0);
    EXPECT_DOUBLE_EQ(model1.step(0.0), 0.0);
    EXPECT_DOUBLE_EQ(model2.step(0.0), 0.0);
    EXPECT_DOUBLE_EQ(model3.step(0.0), 0.0);
}

// Параметризованные тесты
class Model1_1ParamTest : public ::testing::TestWithParam<std::tuple<double, double, double, double, double>> {};

TEST_P(Model1_1ParamTest, ComputesCorrectly) {
    auto [a, b, y0, u, expected_y1] = GetParam();
    Model1_1 model(a, b, y0);
    EXPECT_NEAR(model.step(u), expected_y1, 1e-9);
}

INSTANTIATE_TEST_SUITE_P(
    DefaultValues,
    Model1_1ParamTest,
    ::testing::Values(
        std::make_tuple(0.5, 1.0, 0.0, 1.0, 1.0),
        std::make_tuple(0.0, 1.0, 5.0, 2.0, 2.0)
    )
);