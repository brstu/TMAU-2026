#include "StabilityAnalyzer.hpp"
#include <gtest/gtest.h>
#include <cmath>

using namespace stability;


TEST(FindRootsTest, LinearRoot) {
    auto roots = findRoots({ 1.0, -3.0 });
    ASSERT_EQ(roots.size(), 1u);
    EXPECT_NEAR(roots[0].real(), 3.0, 1e-9);
    EXPECT_NEAR(roots[0].imag(), 0.0, 1e-9);
}

TEST(FindRootsTest, QuadraticRealRoots) {
    auto roots = findRoots({ 1.0, 0.0, -1.0 });
    ASSERT_EQ(roots.size(), 2u);
    EXPECT_NEAR(std::abs(roots[0]), 1.0, 1e-9);
    EXPECT_NEAR(std::abs(roots[1]), 1.0, 1e-9);
}

TEST(FindRootsTest, ComplexPair) {
    auto roots = findRoots({ 1.0, 0.0, 1.0 });
    ASSERT_EQ(roots.size(), 2u);
    EXPECT_NEAR(std::abs(roots[0]), 1.0, 1e-9);
    EXPECT_NEAR(std::abs(roots[1]), 1.0, 1e-9);
    EXPECT_NEAR(std::abs(roots[0].imag()), 1.0, 1e-9);
}

TEST(FindRootsTest, TripleZeroRoot) {
    auto roots = findRoots({ 1.0, 0.0, 0.0, 0.0 });
    ASSERT_EQ(roots.size(), 3u);
    for (const auto& z : roots) EXPECT_LT(std::abs(z), 1e-9);
}


TEST(SpectralRadiusTest, CubicKnownRoots) {
    double r = spectralRadius({ 1.0, 0.0, -0.25, 0.0 });
    EXPECT_NEAR(r, 0.5, 1e-9);
}


TEST(AnalyzeModel16Test, StableCoefficients) {
    auto res = analyzeModel16(0.5, 0.0, 0.0);
    EXPECT_TRUE(res.stable);
    EXPECT_NEAR(res.radius, 0.5, 1e-9);
}

TEST(AnalyzeModel16Test, UnstableCoefficients) {
    auto res = analyzeModel16(2.0, 0.0, 0.0);
    EXPECT_FALSE(res.stable);
    EXPECT_NEAR(res.radius, 2.0, 1e-9);
}

TEST(AnalyzeModel16Test, UnitCircleBoundaryIsNotStable) {
    auto res = analyzeModel16(1.0, 0.0, 0.0);
    EXPECT_FALSE(res.stable);
    EXPECT_NEAR(res.radius, 1.0, 1e-6);
}

TEST(AnalyzeModel16Test, ZeroCoefficients) {
    auto res = analyzeModel16(0.0, 0.0, 0.0);
    EXPECT_TRUE(res.stable);
    EXPECT_NEAR(res.radius, 0.0, 1e-9);
}

TEST(AnalyzeModel16Test, StableComplexPair) {
    auto res = analyzeModel16(0.0, 0.25, 0.0);
    EXPECT_TRUE(res.stable);
    EXPECT_NEAR(res.radius, 0.5, 1e-9);
}

TEST(AnalyzeModel16Test, UnstableComplexPair) {
    auto res = analyzeModel16(0.0, -4.0, 0.0);
    EXPECT_FALSE(res.stable);
    EXPECT_NEAR(res.radius, 2.0, 1e-9);
}

TEST(AnalyzeModel16Test, ThirdOrderRotational) {
    auto res = analyzeModel16(0.0, 0.0, 0.125);
    EXPECT_TRUE(res.stable);
    EXPECT_NEAR(res.radius, 0.5, 1e-9);
}

TEST(AnalyzeModel16Test, MessageIsNotEmpty) {
    auto res = analyzeModel16(0.5, 0.0, 0.0);
    EXPECT_FALSE(res.message.empty());
    EXPECT_NE(res.message.find("STABLE"), std::string::npos);
}