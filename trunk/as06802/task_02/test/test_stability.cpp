#include "StabilityAnalyzer.h"

#include <gtest/gtest.h>

#include <cmath>
#include <limits>
#include <sstream>
#include <stdexcept>

namespace var17 {
namespace {

struct LinearPoleSample {
    const char* tag;
    double coeffA;
    PoleMode expectedMode;
};

class LinearPoleInspectionTest : public ::testing::TestWithParam<LinearPoleSample> {};

TEST_P(LinearPoleInspectionTest, ClassifiesPoleLocationCorrectly) {
    const auto& sample = GetParam();
    const auto report = inspectLinearStability(sample.coeffA);
    EXPECT_DOUBLE_EQ(report.poleLocation, sample.coeffA);
    EXPECT_EQ(report.mode, sample.expectedMode);
    EXPECT_EQ(report.isConvergent(), sample.expectedMode == PoleMode::AsymptoticallyStable);
}

INSTANTIATE_TEST_SUITE_P(Variant17Poles, LinearPoleInspectionTest, ::testing::Values(
    LinearPoleSample{"OriginPole", 0.0, PoleMode::AsymptoticallyStable},
    LinearPoleSample{"DampedNegative", -0.65, PoleMode::AsymptoticallyStable},
    LinearPoleSample{"NearUnitCircleInside", std::nextafter(1.0, 0.0), PoleMode::AsymptoticallyStable},
    LinearPoleSample{"UnitCirclePositive", 1.0, PoleMode::BoundaryMarginal},
    LinearPoleSample{"UnitCircleNegative", -1.0, PoleMode::BoundaryMarginal},
    LinearPoleSample{"NearUnitCircleOutside", std::nextafter(1.0, 2.0), PoleMode::DivergentUnstable},
    LinearPoleSample{"UnstableNegative", -1.8, PoleMode::DivergentUnstable}
), [](const auto& info) { return info.param.tag; });

TEST(LinearStabilityTest, RejectsNonFiniteParameter) {
    EXPECT_THROW(inspectLinearStability(std::numeric_limits<double>::infinity()),
                 std::invalid_argument);
    EXPECT_THROW(inspectLinearStability(std::numeric_limits<double>::quiet_NaN()),
                 std::invalid_argument);
}

TEST(StabilityAlertTest, OutputsNothingWhenStable) {
    std::ostringstream buffer;
    printStabilityAlert(buffer, inspectLinearStability(0.4));
    EXPECT_TRUE(buffer.str().empty());
}

TEST(StabilityAlertTest, FormatsAlertsForMarginalAndUnstableModes) {
    std::ostringstream marginalOut;
    printStabilityAlert(marginalOut, inspectLinearStability(1.0));
    EXPECT_NE(marginalOut.str().find("границе устойчивости"), std::string::npos);
    EXPECT_NE(marginalOut.str().find("z = 1"), std::string::npos);

    std::ostringstream unstableOut;
    printStabilityAlert(unstableOut, inspectLinearStability(-1.4));
    EXPECT_NE(unstableOut.str().find("линейная модель неустойчива"), std::string::npos);
    EXPECT_NE(unstableOut.str().find("z = -1.4"), std::string::npos);
}

} // namespace
} // namespace var17
