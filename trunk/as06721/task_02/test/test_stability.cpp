#include "StabilityAnalyzer.h"

#include <gtest/gtest.h>

#include <cmath>
#include <limits>
#include <sstream>
#include <stdexcept>

namespace task02 {
namespace {

struct PoleCase {
    const char* name;
    double a;
    Stability expected;
};

class StabilityAnalysis : public ::testing::TestWithParam<PoleCase> {};

TEST_P(StabilityAnalysis, ReturnsPoleAndClassifiesItsModulus) {
    const auto& test = GetParam();
    const auto result = analyzeDelayedControl(test.a);
    EXPECT_DOUBLE_EQ(result.root, test.a);
    EXPECT_EQ(result.classification, test.expected);
    EXPECT_EQ(result.isAsymptoticallyStable(), test.expected == Stability::Stable);
}

INSTANTIATE_TEST_SUITE_P(Variant21, StabilityAnalysis, ::testing::Values(
    PoleCase{"ZeroPole", 0.0, Stability::Stable},
    PoleCase{"NegativeStable", -0.75, Stability::Stable},
    PoleCase{"JustInside", std::nextafter(1.0, 0.0), Stability::Stable},
    PoleCase{"PositiveBoundary", 1.0, Stability::Marginal},
    PoleCase{"NegativeBoundary", -1.0, Stability::Marginal},
    PoleCase{"JustOutside", std::nextafter(1.0, 2.0), Stability::Unstable},
    PoleCase{"NegativeUnstable", -1.5, Stability::Unstable}
), [](const auto& info) { return info.param.name; });

TEST(StabilityAnalyzerTest, RejectsNonFiniteCoefficient) {
    EXPECT_THROW(analyzeDelayedControl(std::numeric_limits<double>::infinity()),
                 std::invalid_argument);
    EXPECT_THROW(analyzeDelayedControl(std::numeric_limits<double>::quiet_NaN()),
                 std::invalid_argument);
}

TEST(StabilityWarningTest, StableCaseIsSilent) {
    std::ostringstream message;
    writeStabilityWarning(message, analyzeDelayedControl(0.5));
    EXPECT_TRUE(message.str().empty());
}

TEST(StabilityWarningTest, DescribesBoundaryAndDivergence) {
    std::ostringstream boundary;
    writeStabilityWarning(boundary, analyzeDelayedControl(-1.0));
    EXPECT_NE(boundary.str().find("граница устойчивости"), std::string::npos);
    EXPECT_NE(boundary.str().find("z = -1"), std::string::npos);

    std::ostringstream unstable;
    writeStabilityWarning(unstable, analyzeDelayedControl(1.25));
    EXPECT_NE(unstable.str().find("модель неустойчива"), std::string::npos);
    EXPECT_NE(unstable.str().find("z = 1.25"), std::string::npos);
}

} // namespace
} // namespace task02
