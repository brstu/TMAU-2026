#include "StabilityAnalyzer.h"

#include <gtest/gtest.h>

#include <limits>
#include <sstream>
#include <stdexcept>

namespace var16 {
namespace {

struct ThirdOrderStabilityCase {
    const char* title;
    double a1;
    double a2;
    double a3;
    StabilityZone expectedZone;
};

class JuryCriterionSuite : public ::testing::TestWithParam<ThirdOrderStabilityCase> {};

TEST_P(JuryCriterionSuite, ClassifiesThirdOrderCharacteristicPolynomial) {
    const auto& tc = GetParam();
    const auto report = evaluateThirdOrderStability(tc.a1, tc.a2, tc.a3);
    EXPECT_EQ(report.zone, tc.expectedZone);
    EXPECT_EQ(report.isStable(), tc.expectedZone == StabilityZone::AsymptoticallyStable);
}

INSTANTIATE_TEST_SUITE_P(Variant16Stability, JuryCriterionSuite, ::testing::Values(
    ThirdOrderStabilityCase{"AllPolesAtOrigin", 0.0, 0.0, 0.0, StabilityZone::AsymptoticallyStable},
    ThirdOrderStabilityCase{"DampedThirdOrder", 0.5, 0.25, 0.125, StabilityZone::AsymptoticallyStable},
    ThirdOrderStabilityCase{"AlternatingStable", -0.4, 0.2, -0.1, StabilityZone::AsymptoticallyStable},
    ThirdOrderStabilityCase{"UnitRootAtPlusOne", 0.5, 0.25, 0.25, StabilityZone::CriticalBoundary},
    ThirdOrderStabilityCase{"UnitRootAtMinusOne", -0.5, 0.25, -0.25, StabilityZone::CriticalBoundary},
    ThirdOrderStabilityCase{"PositiveDivergence", 1.2, 0.1, 0.05, StabilityZone::UnstableDivergent},
    ThirdOrderStabilityCase{"OuterCoeffExceedsUnity", 0.0, 0.0, 1.2, StabilityZone::UnstableDivergent},
    ThirdOrderStabilityCase{"JuryMarginViolated", 0.2, -0.9, 0.5, StabilityZone::UnstableDivergent}
), [](const auto& info) { return info.param.title; });

TEST(ThirdOrderStabilityTest, RejectsNonFiniteCoefficients) {
    const double inf = std::numeric_limits<double>::infinity();
    const double nanVal = std::numeric_limits<double>::quiet_NaN();
    EXPECT_THROW(evaluateThirdOrderStability(inf, 0.1, 0.1), std::invalid_argument);
    EXPECT_THROW(evaluateThirdOrderStability(0.1, nanVal, 0.1), std::invalid_argument);
    EXPECT_THROW(evaluateThirdOrderStability(0.1, 0.1, inf), std::invalid_argument);
}

TEST(StabilityReportOutputTest, ProducesNoOutputForStableSystem) {
    std::ostringstream out;
    reportStabilityStatus(out, evaluateThirdOrderStability(0.4, 0.2, 0.1));
    EXPECT_TRUE(out.str().empty());
}

TEST(StabilityReportOutputTest, PrintsWarningsForBoundaryAndUnstableSystems) {
    std::ostringstream boundaryStream;
    reportStabilityStatus(boundaryStream, evaluateThirdOrderStability(0.5, 0.25, 0.25));
    EXPECT_NE(boundaryStream.str().find("границе устойчивости"), std::string::npos);

    std::ostringstream unstableStream;
    reportStabilityStatus(unstableStream, evaluateThirdOrderStability(1.5, 0.0, 0.0));
    EXPECT_NE(unstableStream.str().find("неустойчива на Z-плоскости"), std::string::npos);
}

} // namespace
} // namespace var16
