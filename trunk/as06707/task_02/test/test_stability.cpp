#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "StabilityAnalyzer.h"

// Тестирование граничных значений и логики модуля
TEST(StabilityTests, StableCoefficients) {
    EXPECT_TRUE(StabilityAnalyzer::isLinearModelStable(0.0));
    EXPECT_TRUE(StabilityAnalyzer::isLinearModelStable(0.99));
    EXPECT_TRUE(StabilityAnalyzer::isLinearModelStable(-0.5));
}

TEST(StabilityTests, BoundaryValues) {
    // Граница Z-плоскости |z| = 1
    EXPECT_FALSE(StabilityAnalyzer::isLinearModelStable(1.0));
    EXPECT_FALSE(StabilityAnalyzer::isLinearModelStable(-1.0));
}

TEST(StabilityTests, UnstableCoefficients) {
    EXPECT_FALSE(StabilityAnalyzer::isLinearModelStable(1.1));
    std::string warning = StabilityAnalyzer::getWarningMessage(1.5);
    EXPECT_THAT(warning, ::testing::HasSubstr("ВНИМАНИЕ"));
}

// Покрываем ветку, когда система устойчива и предупреждения нет (для 100% покрытия)
TEST(StabilityTests, StableWarningIsEmpty) {
    std::string warning = StabilityAnalyzer::getWarningMessage(0.5);
    EXPECT_TRUE(warning.empty()); 
}