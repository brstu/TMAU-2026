#include <gtest/gtest.h>

#include <algorithm>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

#include "StabilityAnalyzer.h"

TEST(StabilityTest, StableSystem) {
    // Коэффициенты варианта 19 при a = 1: a1 = 0.4, a2 = 0.3, a3 = 0.1
    EXPECT_TRUE(StabilityAnalyzer::isStable(0.4, 0.3, 0.1));
    EXPECT_TRUE(StabilityAnalyzer::isStableJury(0.4, 0.3, 0.1));
}

TEST(StabilityTest, UnstableSystem) {
    EXPECT_FALSE(StabilityAnalyzer::isStable(2.0, 0.0, 0.0));
    EXPECT_FALSE(StabilityAnalyzer::isStableJury(2.0, 0.0, 0.0));
}

TEST(StabilityTest, BoundaryValues) {
    // Граница Z-плоскости |z| = 1 считается неустойчивой
    EXPECT_FALSE(StabilityAnalyzer::isStable(1.0, 0.0, 0.0));
    EXPECT_FALSE(StabilityAnalyzer::isStable(-1.0, 0.0, 0.0));
    EXPECT_FALSE(StabilityAnalyzer::isStable(0.0, 1.0, 0.0));  // корни 0, +1, -1
}

TEST(StabilityTest, ZeroCoefficients) {
    const auto roots = StabilityAnalyzer::cubicRoots(0.0, 0.0, 0.0);
    EXPECT_NEAR(std::abs(roots.z1), 0.0, 1e-9);
    EXPECT_NEAR(std::abs(roots.z2), 0.0, 1e-9);
    EXPECT_NEAR(std::abs(roots.z3), 0.0, 1e-9);
    EXPECT_TRUE(StabilityAnalyzer::isStable(0.0, 0.0, 0.0));
}

TEST(StabilityTest, RootsOfCharacteristicEquation) {
    // Полином с известными корнями 0.2, -0.3, 0.5:
    // z^3 - 0.4*z^2 + 0.11*z - 0.03  ==>  a1 = 0.4, a2 = 0.11, a3 = -0.03
    const auto roots = StabilityAnalyzer::cubicRoots(0.4, 0.11, -0.03);
    std::vector realParts{ roots.z1.real(), roots.z2.real(), roots.z3.real() }; // Исправлено здесь
    std::sort(realParts.begin(), realParts.end());

    EXPECT_NEAR(realParts[0], -0.3, 1e-8);
    EXPECT_NEAR(realParts[1], 0.2, 1e-8);
    EXPECT_NEAR(realParts[2], 0.5, 1e-8);
    EXPECT_NEAR(std::abs(roots.z1.imag()), 0.0, 1e-8);
    EXPECT_NEAR(std::abs(roots.z2.imag()), 0.0, 1e-8);
    EXPECT_NEAR(std::abs(roots.z3.imag()), 0.0, 1e-8);
}

TEST(StabilityTest, ComplexRootsInsideUnitCircle) {
    // Корни: 0.5, 0.2 +/- 0.4i --> a1 = 0.9, a2 = -0.4, a3 = 0.1
    const auto roots = StabilityAnalyzer::cubicRoots(0.9, -0.4, 0.1);
    EXPECT_TRUE(StabilityAnalyzer::isStable(0.9, -0.4, 0.1));

    const double m1 = std::abs(roots.z1);
    const double m2 = std::abs(roots.z2);
    const double m3 = std::abs(roots.z3);
    EXPECT_NEAR(std::min({ m1, m2, m3 }), 0.4472136, 1e-6);
    EXPECT_NEAR(std::max({ m1, m2, m3 }), 0.5, 1e-6);
}

TEST(StabilityTest, JuryMatchesRootCriterion) {
    const std::vector<std::tuple<double, double, double>> cases{
        {0.4,  0.3,  0.1},
        {0.9, -0.4,  0.1},
        {0.0,  0.0,  0.0},
        {2.0,  0.0,  0.0},
        {1.0,  0.0,  0.0},
        {-1.0, 0.0,  0.0},
        {0.0,  1.0,  0.0},
        {0.4,  0.11, -0.03}
    };

    for (const auto& [a1, a2, a3] : cases) {
        EXPECT_EQ(StabilityAnalyzer::isStable(a1, a2, a3),
            StabilityAnalyzer::isStableJury(a1, a2, a3));
    }
}

TEST(StabilityTest, WarningForUnstableProcess) {
    std::ostringstream output;
    const bool stable = StabilityAnalyzer::warnIfUnstable(2.0, 0.0, 0.0, output);

    EXPECT_FALSE(stable);
    EXPECT_NE(output.str().find("[WARNING]"), std::string::npos);
    EXPECT_NE(output.str().find("z1 ="), std::string::npos);
}

TEST(StabilityTest, NoWarningForStableProcess) {
    std::ostringstream output;
    const bool stable = StabilityAnalyzer::warnIfUnstable(0.4, 0.3, 0.1, output);

    EXPECT_TRUE(stable);
    EXPECT_TRUE(output.str().empty());
}