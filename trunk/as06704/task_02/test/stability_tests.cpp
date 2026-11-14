#include "stability_analyzer.h"
#include "model.h"

#include <gtest/gtest.h>

#include <cmath>
#include <limits>
#include <string>

TEST(StabilityAnalyzerTest, PositiveRootInsideUnitCircleIsStable) {
    const auto report = StabilityAnalyzer::analyzeModel12(0.8, 1);
    EXPECT_EQ(report.status, StabilityStatus::AsymptoticallyStable);
    EXPECT_TRUE(report.isAsymptoticallyStable());
    EXPECT_NEAR(report.characteristicRoot.real(), 0.8, 1e-12);
    EXPECT_NEAR(std::abs(report.characteristicRoot), 0.8, 1e-12);
}

TEST(StabilityAnalyzerTest, NegativeRootInsideUnitCircleIsStable) {
    const auto report = StabilityAnalyzer::analyzeModel12(-0.8, 2);
    EXPECT_EQ(report.status, StabilityStatus::AsymptoticallyStable);
    EXPECT_TRUE(report.isAsymptoticallyStable());
}

TEST(StabilityAnalyzerTest, PositiveRootOutsideUnitCircleIsUnstable) {
    const auto report = StabilityAnalyzer::analyzeModel12(1.2, 1);
    EXPECT_EQ(report.status, StabilityStatus::Unstable);
    EXPECT_FALSE(report.isAsymptoticallyStable());

    LinearModel12 model(1.2, 0.2, 1);
    EXPECT_NE(model.stabilityInfo().find("ПРЕДУПРЕЖДЕНИЕ"), std::string::npos);
}

TEST(StabilityAnalyzerTest, NegativeRootOutsideUnitCircleIsUnstable) {
    const auto report = StabilityAnalyzer::analyzeModel12(-1.2, 1);
    EXPECT_EQ(report.status, StabilityStatus::Unstable);
}

TEST(StabilityAnalyzerTest, PositiveUnitCircleBoundaryIsMarginal) {
    const auto report = StabilityAnalyzer::analyzeModel12(1.0, 1);
    EXPECT_EQ(report.status, StabilityStatus::MarginallyStable);
    EXPECT_FALSE(report.isAsymptoticallyStable());
}

TEST(StabilityAnalyzerTest, NegativeUnitCircleBoundaryIsMarginal) {
    const auto report = StabilityAnalyzer::analyzeModel12(-1.0, 1);
    EXPECT_EQ(report.status, StabilityStatus::MarginallyStable);
    EXPECT_FALSE(report.isAsymptoticallyStable());
}

TEST(StabilityAnalyzerTest, InputDelayDoesNotChangeCharacteristicRoot) {
    const auto first = StabilityAnalyzer::analyzeModel12(0.65, 1);
    const auto third = StabilityAnalyzer::analyzeModel12(0.65, 3);
    const auto tenth = StabilityAnalyzer::analyzeModel12(0.65, 10);

    EXPECT_NEAR(first.characteristicRoot.real(), third.characteristicRoot.real(), 1e-12);
    EXPECT_NEAR(first.characteristicRoot.real(), tenth.characteristicRoot.real(), 1e-12);
    EXPECT_EQ(first.status, third.status);
    EXPECT_EQ(first.status, tenth.status);
}

TEST(StabilityAnalyzerTest, NonFiniteCoefficientIsInvalid) {
    const auto report = StabilityAnalyzer::analyzeModel12(
        std::numeric_limits<double>::quiet_NaN(), 1
    );
    EXPECT_EQ(report.status, StabilityStatus::InvalidParameters);
    EXPECT_FALSE(report.isAsymptoticallyStable());
}


TEST(StabilityAnalyzerTest, StableReportContainsStableDescription) {
    const auto report = StabilityAnalyzer::analyzeModel12(0.5, 1);
    const std::string text = report.message();
    EXPECT_NE(text.find("свободная составляющая затухает"), std::string::npos);
}

TEST(StabilityAnalyzerTest, MarginalReportContainsBoundaryWarning) {
    const auto report = StabilityAnalyzer::analyzeModel12(-1.0, 2);
    const std::string text = report.message();
    EXPECT_NE(text.find("|z| = 1"), std::string::npos);
    EXPECT_NE(text.find("ПРЕДУПРЕЖДЕНИЕ"), std::string::npos);
}

TEST(StabilityAnalyzerTest, UnstableReportContainsDivergenceWarning) {
    const auto report = StabilityAnalyzer::analyzeModel12(1.5, 1);
    const std::string text = report.message();
    EXPECT_NE(text.find("|z| > 1"), std::string::npos);
    EXPECT_NE(text.find("возрастает"), std::string::npos);
}

TEST(StabilityAnalyzerTest, ZeroDelayProducesInvalidParametersMessage) {
    const auto report = StabilityAnalyzer::analyzeModel12(0.5, 0);
    EXPECT_EQ(report.status, StabilityStatus::InvalidParameters);
    EXPECT_NE(report.message().find("недопустимые параметры"), std::string::npos);
}

TEST(StabilityAnalyzerTest, InfiniteCoefficientIsInvalid) {
    const auto report = StabilityAnalyzer::analyzeModel12(
        std::numeric_limits<double>::infinity(), 1
    );
    EXPECT_EQ(report.status, StabilityStatus::InvalidParameters);
}
