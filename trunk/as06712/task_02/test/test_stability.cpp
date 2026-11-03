#include <gtest/gtest.h>
#include "StabilityAnalyzer.h"

TEST(StabilityTest, StableSystem) {
    EXPECT_TRUE(StabilityAnalyzer::isStableLinear13(0.5, -0.2));
}

TEST(StabilityTest, UnstableSystem) {
    EXPECT_FALSE(StabilityAnalyzer::isStableLinear13(2.0, 0.0));
}

TEST(StabilityTest, BoundaryValues) {
    EXPECT_FALSE(StabilityAnalyzer::isStableLinear13(1.0, 0.0));
    EXPECT_FALSE(StabilityAnalyzer::isStableLinear13(-1.0, 0.0));
}