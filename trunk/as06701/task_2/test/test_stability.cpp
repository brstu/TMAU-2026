#include <gtest/gtest.h>
#include "stability_analyzer.h"
#include <cmath>

TEST(Stability, StableFirstOrder) {
    auto r = StabilityAnalyzer::analyze({0.5, 1.0});  // z + 0.5 = 0
    EXPECT_TRUE(r.stable);
    ASSERT_EQ(r.roots.size(), 1u);
    EXPECT_NEAR(std::abs(r.roots[0]), 0.5, 1e-9);
}

TEST(Stability, UnstableFirstOrder) {
    auto r = StabilityAnalyzer::analyze({2.0, 1.0});  // z + 2 = 0
    EXPECT_FALSE(r.stable);
    EXPECT_GT(r.maxMagnitude, 1.0);
}

// Граница единичной окружности |z| = 1
TEST(Stability, BoundaryUnitCircle) {
    auto r = StabilityAnalyzer::analyze({-1.0, 1.0});  // z − 1 = 0
    EXPECT_FALSE(r.stable);
    ASSERT_EQ(r.roots.size(), 1u);
    EXPECT_NEAR(std::abs(r.roots[0]), 1.0, 1e-9);
}

// Model 1.5 default: a1=0.6, a2=0.2, k=2 — устойчива
TEST(Stability, Model15DefaultStable) {
    auto r = StabilityAnalyzer::analyzeModel15(0.6, 0.2, 2);
    EXPECT_TRUE(r.stable) << r.reason;
}

// Model 1.5 расходящаяся: a1=1.5
TEST(Stability, Model15Unstable) {
    auto r = StabilityAnalyzer::analyzeModel15(1.5, 0.2, 2);
    EXPECT_FALSE(r.stable) << r.reason;
}

// Model 1.5 при k=0: z = a1+a2
TEST(Stability, Model15KZero) {
    EXPECT_TRUE (StabilityAnalyzer::analyzeModel15(0.5, 0.3, 0).stable);
    EXPECT_FALSE(StabilityAnalyzer::analyzeModel15(0.8, 0.5, 0).stable);
}

// Model 2.7 — |a| < 1 устойчива, |a| > 1 нет
TEST(Stability, Model27Stable)   { EXPECT_TRUE (StabilityAnalyzer::analyzeModel27(0.7).stable); }
TEST(Stability, Model27Unstable) { EXPECT_FALSE(StabilityAnalyzer::analyzeModel27(1.2).stable); }

// Model 3.9 — корень z=1, маргинально
TEST(Stability, Model39Marginal) {
    auto r = StabilityAnalyzer::analyzeModel39();
    EXPECT_FALSE(r.stable);
    EXPECT_NEAR(r.maxMagnitude, 1.0, 1e-9);
}

// Проверка, что критические коэффициенты не «съедаются» допуском
TEST(Stability, NearBoundary) {
    auto r = StabilityAnalyzer::analyze({-0.999, 1.0});   // z − 0.999 = 0
    EXPECT_TRUE(r.stable);
    auto r2 = StabilityAnalyzer::analyze({-1.001, 1.0});  // z − 1.001 = 0
    EXPECT_FALSE(r2.stable);
}
