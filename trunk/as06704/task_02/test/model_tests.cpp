#include "model.h"

#include <gtest/gtest.h>

#include <stdexcept>
#include <string>
#include <vector>

TEST(LinearModel12Test, AppliesInputAfterConfiguredDelay) {
    const LinearModel12 model(0.5, 2.0, 2);
    const std::vector<double> y{1.0, 0.0, 0.0};
    const std::vector<double> u{1.0, 2.0, 3.0};

    EXPECT_DOUBLE_EQ(model.nextStep(0, y, u, 1.0), 0.5);
    EXPECT_DOUBLE_EQ(model.nextStep(2, y, u, 1.0), 2.0);
}

TEST(LinearModel12Test, DoesNotUseInputBeforeDelayExpires) {
    const LinearModel12 model(0.5, 2.0, 3);
    const std::vector<double> y{1.0, 2.0};
    const std::vector<double> u{10.0, 20.0};

    EXPECT_DOUBLE_EQ(model.nextStep(1, y, u, 1.0), 1.0);
}

TEST(LinearModel12Test, RejectsZeroDelay) {
    EXPECT_THROW(LinearModel12(0.8, 0.2, 0), std::invalid_argument);
}

TEST(NonlinearModel26Test, ComputesPolynomialEquationCorrectly) {
    const NonlinearModel26 model(0.1, 0.2, 0.5);
    const std::vector<double> y{1.0, 2.0, 0.0};
    const std::vector<double> u{0.0, 2.0, 0.0};

    EXPECT_NEAR(model.nextStep(1, y, u, 1.0), 2.6, 1e-12);
}

TEST(NonlinearModel26Test, ZeroStateAndZeroInputRemainAtRest) {
    const NonlinearModel26 model(0.1, 0.2, 0.5);
    const std::vector<double> y{0.0, 0.0};
    const std::vector<double> u{0.0, 0.0};

    EXPECT_DOUBLE_EQ(model.nextStep(0, y, u, 1.0), 0.0);
}

TEST(DifferentialModel310Test, ComputesOneEulerStepCorrectly) {
    const DifferentialModel310 model(0.5, 0.2);
    const std::vector<double> y{2.0};
    const std::vector<double> u{0.4};

    EXPECT_NEAR(model.nextStep(0, y, u, 0.1), 1.96, 1e-12);
}

TEST(DifferentialModel310Test, ConstantOffsetChangesZeroInitialState) {
    const DifferentialModel310 model(0.5, 0.2);
    const std::vector<double> y{0.0};
    const std::vector<double> u{0.0};

    EXPECT_NEAR(model.nextStep(0, y, u, 0.1), 0.02, 1e-12);
}


namespace {

class BasicTestModel final : public Model {
public:
    std::string name() const override {
        return "Basic test model";
    }

    double nextStep(std::size_t tau,
                    const std::vector<double>& y,
                    const std::vector<double>&,
                    double) const override {
        return y[tau];
    }
};

} // namespace

TEST(ModelTest, BaseStabilityInfoIsEmptyByDefault) {
    const BasicTestModel model;
    EXPECT_TRUE(model.stabilityInfo().empty());
}

TEST(LinearModel12Test, ReturnsModelName) {
    const LinearModel12 model(0.8, 0.2, 1);
    EXPECT_NE(model.name().find("Model 1.2"), std::string::npos);
}

TEST(LinearModel12Test, StabilityInfoReportsStableSystem) {
    const LinearModel12 model(0.8, 0.2, 1);
    const std::string info = model.stabilityInfo();
    EXPECT_NE(info.find("Условие выполнено"), std::string::npos);
    EXPECT_EQ(info.find("ПРЕДУПРЕЖДЕНИЕ"), std::string::npos);
}

TEST(LinearModel12Test, StabilityInfoReportsBoundarySystem) {
    const LinearModel12 model(1.0, 0.2, 1);
    const std::string info = model.stabilityInfo();
    EXPECT_NE(info.find("границе устойчивости"), std::string::npos);
    EXPECT_NE(info.find("ПРЕДУПРЕЖДЕНИЕ"), std::string::npos);
}

TEST(NonlinearModel26Test, ReturnsModelName) {
    const NonlinearModel26 model(0.1, 0.2, 0.5);
    EXPECT_NE(model.name().find("Model 2.6"), std::string::npos);
}

TEST(DifferentialModel310Test, ReturnsModelName) {
    const DifferentialModel310 model(0.5, 0.2);
    EXPECT_NE(model.name().find("Model 3.10"), std::string::npos);
}
