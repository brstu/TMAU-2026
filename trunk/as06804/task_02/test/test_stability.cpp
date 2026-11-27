#include <gtest/gtest.h>

#include <cmath>
#include <string>

#include "StabilityAnalyzer.hpp"

using stability::analyzeLinearModel14;
using stability::isStable;

TEST(StabilityAnalyzerTest, StableRootInsideUnitCircle) {
    const auto result = analyzeLinearModel14(0.5, 2.0);
    EXPECT_TRUE(result.stable);
    EXPECT_DOUBLE_EQ(result.root, 0.5);
    EXPECT_DOUBLE_EQ(result.radius, 0.5);
}

TEST(StabilityAnalyzerTest, NegativeStableRoot) {
    const auto result = analyzeLinearModel14(-0.9);
    EXPECT_TRUE(result.stable);
    EXPECT_DOUBLE_EQ(result.radius, 0.9);
}

TEST(StabilityAnalyzerTest, UnstableRootOutsideUnitCircle) {
    const auto result = analyzeLinearModel14(2.0);
    EXPECT_FALSE(result.stable);
    EXPECT_DOUBLE_EQ(result.radius, 2.0);
    EXPECT_NE(result.message.find("UNSTABLE"), std::string::npos);
}

TEST(StabilityAnalyzerTest, PositiveUnitCircleBoundaryIsNotStable) {
    const auto result = analyzeLinearModel14(1.0);
    EXPECT_FALSE(result.stable);
    EXPECT_DOUBLE_EQ(result.radius, 1.0);
    EXPECT_NE(result.message.find("MARGINAL"), std::string::npos);
}

TEST(StabilityAnalyzerTest, NegativeUnitCircleBoundaryIsNotStable) {
    const auto result = analyzeLinearModel14(-1.0);
    EXPECT_FALSE(result.stable);
    EXPECT_DOUBLE_EQ(result.radius, 1.0);
    EXPECT_NE(result.message.find("MARGINAL"), std::string::npos);
}

TEST(StabilityAnalyzerTest, ZeroRootIsStable) {
    const auto result = analyzeLinearModel14(0.0);
    EXPECT_TRUE(result.stable);
    EXPECT_DOUBLE_EQ(result.radius, 0.0);
}

TEST(StabilityAnalyzerTest, InputCoefficientDoesNotAffectFreeProcess) {
    EXPECT_TRUE(analyzeLinearModel14(0.5, 1e6).stable);
    EXPECT_FALSE(analyzeLinearModel14(1.5, -1e6).stable);
}

TEST(StabilityAnalyzerTest, IsStableHelperMatchesAnalyticCriterion) {
    EXPECT_TRUE(isStable(0.999999));
    EXPECT_TRUE(isStable(-0.999999));
    EXPECT_FALSE(isStable(1.0));
    EXPECT_FALSE(isStable(-1.0));
    EXPECT_FALSE(isStable(1.000001));
}

TEST(StabilityAnalyzerTest, StableMessageContainsStatus) {
    const auto result = analyzeLinearModel14(0.25);
    EXPECT_FALSE(result.message.empty());
    EXPECT_NE(result.message.find("STABLE"), std::string::npos);
    EXPECT_NE(result.message.find("z = 0.25"), std::string::npos);
}

struct StabilityCase {
    double a;
    bool stable;
};

class StabilityParametricTest : public testing::TestWithParam<StabilityCase> {};

TEST_P(StabilityParametricTest, CriterionMatchesExpected) {
    const auto param = GetParam();
    const auto result = analyzeLinearModel14(param.a);

    EXPECT_EQ(result.stable, param.stable) << "a = " << param.a;
    EXPECT_NEAR(result.radius, std::fabs(param.a), 1e-12);
    EXPECT_NEAR(result.root, param.a, 1e-12);
}

INSTANTIATE_TEST_SUITE_P(
    UnitCircleMatrix,
    StabilityParametricTest,
    testing::Values(
        StabilityCase{0.0, true},
        StabilityCase{0.5, true},
        StabilityCase{-0.5, true},
        StabilityCase{0.999999, true},
        StabilityCase{-0.999999, true},
        StabilityCase{1.0, false},
        StabilityCase{-1.0, false},
        StabilityCase{1.01, false},
        StabilityCase{-1.01, false},
        StabilityCase{2.0, false},
        StabilityCase{-2.0, false}
    )
);
