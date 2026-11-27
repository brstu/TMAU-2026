#include <gtest/gtest.h>

#include <algorithm>
#include <cmath>
#include <complex>
#include <string>
#include <vector>

#include "Model17.h"
#include "Model21.h"
#include "StabilityAnalyzer.h"

namespace {

class MinimalModel : public IModel {
public:
    double step(double u) override {
        (void)u;
        return 0.0;
    }
    void reset() override {}
    std::string name() const override { return "Minimal model"; }
};

class LinearWithoutPolynomialModel : public IModel {
public:
    double step(double u) override {
        (void)u;
        return 0.0;
    }
    void reset() override {}
    std::string name() const override { return "Linear model without polynomial"; }
    bool isLinear() const override { return true; }
};

class SecondOrderLinearModel : public IModel {
public:
    double step(double u) override {
        (void)u;
        return 0.0;
    }
    void reset() override {}
    std::string name() const override { return "Second order model"; }
    bool isLinear() const override { return true; }
    std::vector<double> characteristicPolynomial() const override {
        return {1.0, -2.0, 1.0};
    }
};

struct StabilityCase {
    double a;
    bool stable;
    bool marginal;
};

double polynomialResidual(const std::vector<double>& coefficients,
                          const std::complex<double>& z) {
    std::complex<double> value(0.0, 0.0);
    for (double coefficient : coefficients) {
        value = value * z + coefficient;
    }

    double scale = 0.0;
    double power = 1.0;
    const double zMax = std::max(1.0, std::abs(z));
    for (auto it = coefficients.rbegin(); it != coefficients.rend(); ++it) {
        scale += std::abs(*it) * power;
        power *= zMax;
    }

    if (scale == 0.0) {
        return std::abs(value);
    }
    return std::abs(value) / scale;
}

} // namespace

class Model17StabilityTest : public ::testing::TestWithParam<StabilityCase> {
};

TEST_P(Model17StabilityTest, MatchesAnalyticCriterionForFirstOrderModel) {
    const StabilityCase& testCase = GetParam();
    const StabilityReport report =
        StabilityAnalyzer::analyze(std::vector<double>{1.0, -testCase.a});

    EXPECT_TRUE(report.analyzed);
    EXPECT_EQ(report.stable, testCase.stable);
    EXPECT_EQ(report.marginal, testCase.marginal);
    EXPECT_NEAR(report.maxMagnitude, std::abs(testCase.a), 1e-9);
    ASSERT_EQ(report.roots.size(), 1u);
    EXPECT_NEAR(report.roots.front().real(), testCase.a, 1e-9);
    EXPECT_NEAR(report.roots.front().imag(), 0.0, 1e-9);
}

INSTANTIATE_TEST_SUITE_P(
    UnitCircleBoundary,
    Model17StabilityTest,
    ::testing::Values(
        StabilityCase{0.0, true, false},
        StabilityCase{0.5, true, false},
        StabilityCase{-0.7, true, false},
        StabilityCase{0.999, true, false},
        StabilityCase{1.0, false, true},
        StabilityCase{-1.0, false, true},
        StabilityCase{1.0001, false, false},
        StabilityCase{-1.2, false, false},
        StabilityCase{2.0, false, false}));

TEST(StabilityAnalyzerTest, FindsRootOfFirstOrderPolynomial) {
    const auto roots = StabilityAnalyzer::findRoots({2.0, -1.0});

    ASSERT_EQ(roots.size(), 1u);
    EXPECT_NEAR(roots.front().real(), 0.5, 1e-12);
    EXPECT_NEAR(roots.front().imag(), 0.0, 1e-12);
}

TEST(StabilityAnalyzerTest, FindsRootsOfSecondOrderPolynomial) {
    const auto roots = StabilityAnalyzer::findRoots({1.0, -1.5, 0.56});

    ASSERT_EQ(roots.size(), 2u);

    std::vector<double> magnitudes = {
        std::abs(roots[0]),
        std::abs(roots[1])
    };
    std::sort(magnitudes.begin(), magnitudes.end());

    EXPECT_NEAR(magnitudes[0], 0.7, 1e-9);
    EXPECT_NEAR(magnitudes[1], 0.8, 1e-9);
}

TEST(StabilityAnalyzerTest, HandlesComplexConjugateRoots) {
    const auto roots = StabilityAnalyzer::findRoots({1.0, -1.0, 0.5});

    ASSERT_EQ(roots.size(), 2u);
    EXPECT_NEAR(std::abs(roots[0]), std::sqrt(0.5), 1e-9);
    EXPECT_NEAR(std::abs(roots[1]), std::sqrt(0.5), 1e-9);
    EXPECT_NEAR(roots[0].imag(), -roots[1].imag(), 1e-9);
}

TEST(StabilityAnalyzerTest, DetectsUnstableComplexPair) {
    const StabilityReport report =
        StabilityAnalyzer::analyze(std::vector<double>{1.0, -2.0, 4.0});

    EXPECT_TRUE(report.analyzed);
    EXPECT_FALSE(report.stable);
    EXPECT_FALSE(report.marginal);
    EXPECT_NEAR(report.maxMagnitude, 2.0, 1e-9);
}

TEST(StabilityAnalyzerTest, HandlesRootAtOrigin) {
    const auto roots = StabilityAnalyzer::findRoots({1.0, -0.5, 0.0});

    ASSERT_EQ(roots.size(), 2u);

    std::vector<double> magnitudes = {
        std::abs(roots[0]),
        std::abs(roots[1])
    };
    std::sort(magnitudes.begin(), magnitudes.end());

    EXPECT_NEAR(magnitudes[0], 0.0, 1e-12);
    EXPECT_NEAR(magnitudes[1], 0.5, 1e-12);
}

TEST(StabilityAnalyzerTest, HandlesRepeatedRoot) {
    const StabilityReport report =
        StabilityAnalyzer::analyze(std::vector<double>{1.0, -1.0, 0.25});

    EXPECT_TRUE(report.stable);
    EXPECT_NEAR(report.maxMagnitude, 0.5, 1e-5);
}

TEST(StabilityAnalyzerTest, ReportsDegenerateEquation) {
    const StabilityReport emptyReport = StabilityAnalyzer::analyze(std::vector<double>{});
    EXPECT_FALSE(emptyReport.analyzed);
    EXPECT_TRUE(emptyReport.stable);

    const StabilityReport zeroReport =
        StabilityAnalyzer::analyze(std::vector<double>{0.0, 0.0});
    EXPECT_FALSE(zeroReport.analyzed);
    EXPECT_TRUE(zeroReport.stable);
}

TEST(StabilityAnalyzerTest, ConstantPolynomialHasNoDynamicRoots) {
    const StabilityReport report = StabilityAnalyzer::analyze(std::vector<double>{5.0});

    EXPECT_TRUE(report.analyzed);
    EXPECT_TRUE(report.stable);
    EXPECT_TRUE(report.roots.empty());
}

TEST(StabilityAnalyzerTest, FindsNoRootsForDegenerateInput) {
    EXPECT_TRUE(StabilityAnalyzer::findRoots({}).empty());
    EXPECT_TRUE(StabilityAnalyzer::findRoots({5.0}).empty());
    EXPECT_TRUE(StabilityAnalyzer::findRoots({0.0, 0.0}).empty());
}

TEST(StabilityAnalyzerTest, InterfaceDefaultsAreSafe) {
    MinimalModel model;
    EXPECT_FALSE(model.isLinear());
    EXPECT_TRUE(model.characteristicPolynomial().empty());
}

TEST(StabilityAnalyzerTest, LinearModelWithoutPolynomialIsNotAnalyzed) {
    LinearWithoutPolynomialModel model;
    const StabilityReport report = StabilityAnalyzer::analyze(model);

    EXPECT_TRUE(model.isLinear());
    EXPECT_FALSE(report.analyzed);
    EXPECT_TRUE(report.stable);
    EXPECT_TRUE(report.roots.empty());
}

TEST(StabilityAnalyzerTest, NonlinearModelIsNotAnalyzed) {
    const Model21 model(0.5, 0.05, 0.8, 0.2);
    const StabilityReport report = StabilityAnalyzer::analyze(model);

    EXPECT_FALSE(report.analyzed);
    EXPECT_TRUE(report.stable);
    EXPECT_TRUE(report.roots.empty());
}

TEST(StabilityAnalyzerTest, LinearModelUsesCharacteristicPolynomial) {
    const Model17 stableModel(0.7, 0.5, 0.2, 0.1);
    const Model17 unstableModel(1.5, 0.5, 0.2, 0.1);

    const StabilityReport stableReport =
        StabilityAnalyzer::analyze(stableModel);
    const StabilityReport unstableReport =
        StabilityAnalyzer::analyze(unstableModel);

    EXPECT_TRUE(stableReport.analyzed);
    EXPECT_TRUE(stableReport.stable);
    EXPECT_NEAR(stableReport.maxMagnitude, 0.7, 1e-9);

    EXPECT_TRUE(unstableReport.analyzed);
    EXPECT_FALSE(unstableReport.stable);
    EXPECT_NEAR(unstableReport.maxMagnitude, 1.5, 1e-9);
}

TEST(StabilityAnalyzerTest, WarningContainsModelNameAndReason) {
    const Model17 model(1.5, 0.5, 0.2, 0.1);
    const StabilityReport report = StabilityAnalyzer::analyze(model);
    const std::string warning =
        StabilityAnalyzer::buildWarning(model, report);

    EXPECT_NE(warning.find("WARNING"), std::string::npos);
    EXPECT_NE(warning.find("Model 1.7"), std::string::npos);
    EXPECT_NE(warning.find("НЕУСТОЙЧИВА"), std::string::npos);
    EXPECT_NE(warning.find("|z|max"), std::string::npos);
}

TEST(StabilityAnalyzerTest, WarningContainsAllRoots) {
    const SecondOrderLinearModel model;
    const StabilityReport report = StabilityAnalyzer::analyze(model);
    const std::string warning =
        StabilityAnalyzer::buildWarning(model, report);

    EXPECT_FALSE(report.stable);
    EXPECT_NE(warning.find("z1"), std::string::npos);
    EXPECT_NE(warning.find("z2"), std::string::npos);
    EXPECT_NE(warning.find("Second order model"), std::string::npos);
}

TEST(StabilityAnalyzerTest, StableReportHasNoWarningPrefix) {
    const Model17 model(0.7, 0.5, 0.2, 0.1);
    const StabilityReport report = StabilityAnalyzer::analyze(model);

    EXPECT_TRUE(report.stable);
    EXPECT_EQ(report.marginal, false);
}

TEST(StabilityAnalyzerTest, ThirdOrderRootsSatisfyPolynomial) {
    const std::vector<double> coefficients = {1.0, -0.2, 0.1, -0.3};
    const auto roots = StabilityAnalyzer::findRoots(coefficients);

    ASSERT_EQ(roots.size(), 3u);
    for (const auto& root : roots) {
        EXPECT_LT(polynomialResidual(coefficients, root), 1e-8);
    }
}

TEST(StabilityAnalyzerTest, HighOrderRootsOfUnity) {
    const std::vector<double> coefficients = {1.0, 0.0, 0.0, 0.0, -0.0625};
    const auto roots = StabilityAnalyzer::findRoots(coefficients);

    ASSERT_EQ(roots.size(), 4u);
    for (const auto& root : roots) {
        EXPECT_NEAR(std::abs(root), 0.5, 1e-8);
    }
}

TEST(StabilityAnalyzerTest, RootMagnitudesForKnownComplexPolynomial) {
    const auto roots = StabilityAnalyzer::findRoots({1.0, 0.0, 0.25});
    ASSERT_EQ(roots.size(), 2u);

    for (const auto& root : roots) {
        EXPECT_NEAR(root.real(), 0.0, 1e-9);
        EXPECT_NEAR(std::abs(root.imag()), 0.5, 1e-9);
    }
}

class PolynomialRootsTest : public ::testing::TestWithParam<std::vector<double>> {
};

TEST_P(PolynomialRootsTest, DurandKernerRootsSatisfyPolynomial) {
    const std::vector<double> coefficients = GetParam();
    const auto roots = StabilityAnalyzer::findRoots(coefficients);

    ASSERT_EQ(roots.size(), coefficients.size() - 1);
    for (const auto& root : roots) {
        EXPECT_LT(polynomialResidual(coefficients, root), 1e-6)
            << "root = " << root;
    }
}

INSTANTIATE_TEST_SUITE_P(
    VariousDegrees,
    PolynomialRootsTest,
    ::testing::Values(
        std::vector<double>{1.0, -0.5},
        std::vector<double>{1.0, -1.5, 0.56},
        std::vector<double>{1.0, -1.0, 0.5},
        std::vector<double>{1.0, 0.0, -0.25},
        std::vector<double>{1.0, -0.2, 0.1, -0.3},
        std::vector<double>{1.0, 0.0, 0.0, 0.0, -0.0625},
        std::vector<double>{1.0, 2.0, 3.0, 4.0, 5.0},
        std::vector<double>{1.0, -2.0, 1.0}));
