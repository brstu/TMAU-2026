#include <gtest/gtest.h>

#include <algorithm>
#include <cmath>
#include <sstream>

#include "stability.hpp"

struct StabilityCase {
    double a1;
    double a2;
    double a3;
    bool stable;
};


class StabilityCases : public ::testing::TestWithParam<StabilityCase> {};

TEST_P(StabilityCases, AnalyzerReturnsExpectedResult) {
    const auto [a1, a2, a3, stable] = GetParam();

    EXPECT_EQ(stability::isStable(a1, a2, a3), stable);

    const auto report = stability::analyze(a1, a2, a3);
    EXPECT_EQ(report.stable, stable);
    if (stable) {
        EXPECT_LT(report.spectralRadius, 1.0);
    } else {
        EXPECT_GT(report.spectralRadius, 1.0 - 1e-6);
    }
}

TEST_P(StabilityCases, WarningIsPrintedOnlyForUnstable) {
    const auto [a1, a2, a3, stable] = GetParam();
    std::ostringstream out;

    EXPECT_EQ(stability::warnIfUnstable(a1, a2, a3, out), stable);
    EXPECT_EQ(out.str().empty(), stable);
}

INSTANTIATE_TEST_SUITE_P(
    Coefficients, StabilityCases,
    ::testing::Values(StabilityCase{0.5, 0.2, 0.1, true},      // def coeffs
                      StabilityCase{-0.5, 0.0, 0.0, true},     // z = -0.5
                      StabilityCase{0.0, 0.0, 0.99, true},     // |z| < 1 
                      StabilityCase{1.5, 0.0, 0.0, false},     // z = 1.5
                      StabilityCase{0.0, 0.0, 1.2, false},     // z^3 = 1.2
                      StabilityCase{1.0, 0.0, 0.0, false},     // boundary z = 1
                      StabilityCase{-1.0, 0.0, 0.0, false},    // boundary z = -1
                      StabilityCase{0.0, 0.0, 1.0, false},     // boundary z^3 = 1
                      StabilityCase{1.0, -1.0, 1.0, false}),   // boundary z = 1, +i, -i
    [](const auto& info) { return "Case" + std::to_string(info.index); });

TEST(Stability, CharacteristicRoots) {
    
    // (z - 0.5)(z - 0.2)(z + 0.3) = z^3 - 0.4 z^2 - 0.11 z + 0.03
    
    const auto roots = stability::characteristicRoots(0.4, 0.11, -0.03);

    for (const double expected : {0.5, 0.2, -0.3}) {
        const bool found = std::any_of(roots.begin(), roots.end(), [expected](const auto& r) {
            return std::abs(r - stability::Complex(expected, 0.0)) < 1e-8;
        });
        EXPECT_TRUE(found) << "root " << expected << " not found";
    }
}

TEST(Stability, WarningMentionsUnstableModel) {
    std::ostringstream out;

    stability::warnIfUnstable(1.5, 0.0, 0.0, out);

    EXPECT_NE(out.str().find("ВНИМАНИЕ"), std::string::npos);
}