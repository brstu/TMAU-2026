#include <gtest/gtest.h>

#include <cmath>
#include <sstream>
#include <string>

#include "Model_1_1.h"
#include "StabilityAnalyzer.h"

namespace
{
    template <class T>
    std::string caseName(const testing::TestParamInfo<T>& info)
    {
        return "case" + std::to_string(info.index);
    }

    //для модели 1.1 корень характеристического уравнения z = a.
    //устойчивость: |a| < 1.
    struct StabilityCase
    {
        double a;
        bool expectedStable;
    };

    class StabilityTest : public testing::TestWithParam<StabilityCase> {};

    TEST_P(StabilityTest, IsStableMatchesRootPosition)
    {
        EXPECT_EQ(isStable(GetParam().a), GetParam().expectedStable);
    }

    INSTANTIATE_TEST_SUITE_P(Roots, StabilityTest, testing::Values(
        StabilityCase{ 0.0,  true},
        StabilityCase{ 0.5,  true},
        StabilityCase{-0.5,  true},
        StabilityCase{ 0.99, true},
        StabilityCase{ 1.0,  false},
        StabilityCase{-1.0,  false},
        StabilityCase{ 1.01, false},
        StabilityCase{ 1.5,  false},
        StabilityCase{-2.0,  false}
    ), caseName<StabilityCase>);

    TEST(StabilityTest2, CheckStabilityGivesThreeVerdicts)
    {
        EXPECT_TRUE(checkStability(0.9) == Stability::Stable);
        EXPECT_TRUE(checkStability(1.0) == Stability::Boundary);
        EXPECT_TRUE(checkStability(-1.0) == Stability::Boundary);
        EXPECT_TRUE(checkStability(1.1) == Stability::Unstable);
    }


    TEST(WarningTest, NoWarningWhenStable)
    {
        std::ostringstream out;
        EXPECT_FALSE(warnIfUnstable(0.8, out));
        EXPECT_TRUE(out.str().empty());
    }

    TEST(WarningTest, WarningWhenUnstable)
    {
        std::ostringstream out;
        EXPECT_TRUE(warnIfUnstable(1.5, out));
        EXPECT_NE(out.str().find("ПРЕДУПРЕЖДЕНИЕ"), std::string::npos);
    }

    TEST(WarningTest, WarningOnBoundary)
    {
        std::ostringstream out;
        EXPECT_TRUE(warnIfUnstable(1.0, out));
        EXPECT_NE(out.str().find("границе"), std::string::npos);
    }

    //свободное движение: y(0) = 1, u = 0, поэтому y(t) = a^t.
    double freeMotion(double a, int steps)
    {
        const Model1_1 model(a, 1.0);
        double y = 1.0;
        for (int i = 0; i < steps; ++i)
            y = model.calculateNext(y, 0.0, 1.0);
        return y;
    }

    TEST(BehaviourTest, StableModelDecays)
    {
        EXPECT_LT(std::fabs(freeMotion(0.5, 100)), 1e-6);
    }

    TEST(BehaviourTest, UnstableModelGrows)
    {
        EXPECT_GT(std::fabs(freeMotion(1.5, 20)), 1000.0);
    }
}
