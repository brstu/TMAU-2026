#include <gtest/gtest.h>
#include "../src/stability_analyzer.h"

// 1. Проверка граничного состояния |z| = 1
TEST(StabilityAnalyzerTest, BoundaryCondition) {
    EXPECT_FALSE(StabilityAnalyzer::isStableLinearModel1_4(1.0));
    EXPECT_FALSE(StabilityAnalyzer::isStableLinearModel1_4(-1.0));
}

// 2. Проверка заведомо устойчивых коэффициентов (|a| < 1)
TEST(StabilityAnalyzerTest, StableCoefficients) {
    EXPECT_TRUE(StabilityAnalyzer::isStableLinearModel1_4(0.8));
    EXPECT_TRUE(StabilityAnalyzer::isStableLinearModel1_4(-0.5));
    EXPECT_TRUE(StabilityAnalyzer::isStableLinearModel1_4(0.0));
}

// 3. Проверка критических неустойчивых параметров (|a| > 1)
TEST(StabilityAnalyzerTest, UnstableCoefficients) {
    EXPECT_FALSE(StabilityAnalyzer::isStableLinearModel1_4(1.2));
    EXPECT_FALSE(StabilityAnalyzer::isStableLinearModel1_4(-1.5));
}

// 4. Проверка корректности нахождения корня Z-плоскости
TEST(StabilityAnalyzerTest, RootCalculation) {
    auto roots = StabilityAnalyzer::getRootsLinearModel1_4(0.8);
    ASSERT_EQ(roots.size(), 1u);
    EXPECT_DOUBLE_EQ(roots[0].real(), 0.8);
    EXPECT_DOUBLE_EQ(roots[0].imag(), 0.0);
}

// 5. Проверка функции предупреждения
TEST(StabilityAnalyzerTest, CheckAndWarnOutput) {
    EXPECT_TRUE(StabilityAnalyzer::checkAndWarnStability(0.5));
    EXPECT_FALSE(StabilityAnalyzer::checkAndWarnStability(1.5));
}