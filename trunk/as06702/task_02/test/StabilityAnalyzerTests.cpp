#include "StabilityAnalyzer.h"
#include "gtest/gtest.h"

#include <cmath>
#include <stdexcept>
#include <string>

TEST(StabilityAnalyzerTests, RecognizesStableVariant13Coefficients) {
    const StabilityReport report = StabilityAnalyzer::analyze(1.2, -0.32);
    EXPECT_EQ(report.status, StabilityStatus::AsymptoticallyStable);
    EXPECT_TRUE(report.isAsymptoticallyStable());
    EXPECT_NEAR(std::abs(report.root1), 0.8, 1e-12);
    EXPECT_NEAR(std::abs(report.root2), 0.4, 1e-12);
}

TEST(StabilityAnalyzerTests, RecognizesUnstableCoefficients) {
    const StabilityReport report = StabilityAnalyzer::analyze(1.5, -0.2);
    EXPECT_EQ(report.status, StabilityStatus::Unstable);
    EXPECT_FALSE(report.isAsymptoticallyStable());
}

TEST(StabilityAnalyzerTests, DetectsRootOnUnitCircleBoundary) {
    const StabilityReport report = StabilityAnalyzer::analyze(0.0, -1.0);
    EXPECT_EQ(report.status, StabilityStatus::MarginallyStable);
    EXPECT_NEAR(std::abs(report.root1), 1.0, 1e-12);
    EXPECT_NEAR(std::abs(report.root2), 1.0, 1e-12);
    EXPECT_FALSE(report.isAsymptoticallyStable());
}

TEST(StabilityAnalyzerTests, ProducesWarningForNonStableSystem) {
    const StabilityReport report = StabilityAnalyzer::analyze(0.0, -1.0);
    const std::string text = StabilityAnalyzer::formatReport(report);
    EXPECT_NE(text.find("ПРЕДУПРЕЖДЕНИЕ"), std::string::npos);
}

TEST(StabilityAnalyzerTests, RejectsInvalidTolerance) {
    EXPECT_THROW(StabilityAnalyzer::analyze(0.5, -0.2, 0.0), std::invalid_argument);
}
