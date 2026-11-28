#include <gtest/gtest.h>
#include <cmath>
#include <sstream>
#include <string>
#include "model1-8.h"
#include "stability_core.h"

namespace
{
    template <class T>
    std::string labelOf(const testing::TestParamInfo<T>& info)
    {
        return "case" + std::to_string(info.index);
    }

    struct StabilityCase
    {
        double a1, a2;
        bool expectedStable;
    };
    class RootTest : public testing::TestWithParam<StabilityCase> {};
    TEST_P(RootTest, IsStableMatchesRootPosition)
    {
        EXPECT_EQ(isStable(GetParam().a1, GetParam().a2),
                  GetParam().expectedStable);
    }
    INSTANTIATE_TEST_SUITE_P(Roots, RootTest, testing::Values(
        StabilityCase{0.5, 0.2,  true},   // корни внутри
        StabilityCase{0.0, 0.0,  true},
        StabilityCase{0.0, -0.5, true},   // z = ±sqrt(0.5)
        StabilityCase{1.0, 0.0,  false},  // z = 1 — граница
        StabilityCase{-1.0, 0.0, false},  // z = -1 — граница
        StabilityCase{2.0, 0.0,  false},  // z = 2 — неустойчиво
        StabilityCase{0.0, 1.0,  false},  // z = ±1 — граница
        StabilityCase{0.0, 2.0,  false}   // z = ±sqrt(2) — неустойчиво
    ), labelOf<StabilityCase>);

    TEST(VerdictTest, CheckStabilityGivesThreeVerdicts)
    {
        EXPECT_TRUE(checkStability(0.5, 0.2) == Stability::Stable);
        EXPECT_TRUE(checkStability(1.0, 0.0) == Stability::Boundary);
        EXPECT_TRUE(checkStability(2.0, 0.0) == Stability::Unstable);
    }

    TEST(WarningTest, NoWarningWhenStable)
    {
        std::ostringstream out;
        EXPECT_FALSE(warnIfUnstable(0.5, 0.2, out));
        EXPECT_TRUE(out.str().empty());
    }
    TEST(WarningTest, WarningWhenUnstable)
    {
        std::ostringstream out;
        EXPECT_TRUE(warnIfUnstable(2.0, 0.0, out));
        EXPECT_NE(out.str().find("ПРЕДУПРЕЖДЕНИЕ"), std::string::npos);
    }
    TEST(WarningTest, WarningOnBoundary)
    {
        std::ostringstream out;
        EXPECT_TRUE(warnIfUnstable(1.0, 0.0, out));
        EXPECT_NE(out.str().find("границе"), std::string::npos);
    }

    // Свободное движение: y0 = 1, u = 0 -> процесс затухает для устойчивой модели
    TEST(BehaviourTest, StableModelDecays)
    {
        ArModel m(0.5, 0.2, 0.0, 0.0);
        m.setInitial(1.0);
        double y = 1.0, up1 = 0.0, up2 = 0.0;
        for (int i = 0; i < 200; ++i)
            y = m.calculateStep(0.0, up1, up2);
        EXPECT_LT(std::abs(y), 1e-6);
    }
    TEST(BehaviourTest, UnstableModelGrows)
    {
        ArModel m(2.0, 0.0, 0.0, 0.0);
        m.setInitial(1.0);
        double y = 1.0, up1 = 0.0, up2 = 0.0;
        for (int i = 0; i < 20; ++i)
            y = m.calculateStep(0.0, up1, up2);
        EXPECT_GT(std::abs(y), 1000.0);
    }
}