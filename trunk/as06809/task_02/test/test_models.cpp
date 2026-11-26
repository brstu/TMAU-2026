#include "StabilityAnalyzer.h"

#include <gtest/gtest.h>

#include <cmath>
#include <limits>
#include <memory>
#include <stdexcept>
#include <vector>

namespace var16 {
namespace {

struct ThirdOrderCase {
    const char* name;
    double a1;
    double a2;
    double a3;
    double b;
    std::vector<double> uInput;
    std::vector<double> yExpected;
};

class ThirdOrderTrajectorySuite : public ::testing::TestWithParam<ThirdOrderCase> {};

TEST_P(ThirdOrderTrajectorySuite, MatchesExactDiscreteTrajectory) {
    const auto& tc = GetParam();
    const ThirdOrderLinearModel model(tc.a1, tc.a2, tc.a3, tc.b);
    const int n = static_cast<int>(tc.yExpected.size()) - 1;
    const auto trace = model.execute(n, tc.yExpected.front(), 1.0,
        [&](int k) { return tc.uInput.at(static_cast<std::size_t>(k)); });

    ASSERT_EQ(trace.size(), tc.yExpected.size());
    for (std::size_t i = 0; i < trace.size(); ++i) {
        SCOPED_TRACE(i);
        EXPECT_EQ(trace[i].step, static_cast<int>(i));
        EXPECT_DOUBLE_EQ(trace[i].uVal, tc.uInput[i]);
        EXPECT_NEAR(trace[i].yVal, tc.yExpected[i], 1e-12);
    }
}

INSTANTIATE_TEST_SUITE_P(Variant16Cases, ThirdOrderTrajectorySuite, ::testing::Values(
    ThirdOrderCase{"StepInput", 0.5, 0.25, 0.125, 2.0,
        {1, 1, 1, 1, 1}, {0.0, 2.0, 3.0, 4.0, 5.0}},
    ThirdOrderCase{"ImpulseInput", 0.5, 0.2, 0.1, 4.0,
        {1, 0, 0, 0, 0}, {0.0, 4.0, 2.0, 1.8, 1.7}},
    ThirdOrderCase{"ZeroRest", 0.4, -0.2, 0.1, 3.0,
        {0, 0, 0, 0}, {0.0, 0.0, 0.0, 0.0}},
    ThirdOrderCase{"UnstableGrowth", 2.0, 0.0, 0.0, 0.0,
        {0, 0, 0, 0}, {1.0, 2.0, 4.0, 8.0}}
), [](const auto& info) { return info.param.name; });

TEST(ThirdOrderLinearModelTest, ConvergesToSteadyStateValue) {
    const ThirdOrderLinearModel model(0.5, 0.25, 0.125, 2.0);
    const auto trace = model.execute(400, 0.0, 1.0, createInputFn(InputMode::Step));
    EXPECT_NEAR(trace.back().yVal, 16.0, 1e-12);
}


TEST(ThirdOrderLinearModelTest, RejectsNonFiniteCoefficients) {
    const double inf = std::numeric_limits<double>::infinity();
    EXPECT_THROW(ThirdOrderLinearModel(inf, 0.1, 0.1, 1.0), std::invalid_argument);
    EXPECT_THROW(ThirdOrderLinearModel(0.1, inf, 0.1, 1.0), std::invalid_argument);
    EXPECT_THROW(ThirdOrderLinearModel(0.1, 0.1, inf, 1.0), std::invalid_argument);
    EXPECT_THROW(ThirdOrderLinearModel(0.1, 0.1, 0.1, inf), std::invalid_argument);
}

struct TanhCubicCase {
    const char* name;
    double a;
    double b;
    double y;
    double u;
    double expected;
};

class TanhCubicStepSuite : public ::testing::TestWithParam<TanhCubicCase> {};

TEST_P(TanhCubicStepSuite, EvaluatesNonlinearStepAccurately) {
    const auto& tc = GetParam();
    const TanhCubicNonlinearModel model(tc.a, tc.b);
    EXPECT_NEAR(model.computeStep({tc.y, 0.0, 0.0, tc.u, 1.0}), tc.expected, 1e-12);
}

INSTANTIATE_TEST_SUITE_P(Variant16Cases, TanhCubicStepSuite, ::testing::Values(
    TanhCubicCase{"EquilibriumOrigin", 0.9, 2.5, 0.0, 0.0, 0.0},
    TanhCubicCase{"PureCubicControl", 1.5, 2.0, 0.0, 2.0, 16.0},
    TanhCubicCase{"OddSymmetryNegativeInput", 1.5, 2.0, 0.0, -2.0, -16.0},
    TanhCubicCase{"HyperbolicTangentResponse", 3.0, 0.5, 1.0, 2.0, 3.0 * std::tanh(1.0) + 4.0}
), [](const auto& info) { return info.param.name; });

TEST(TanhCubicNonlinearModelTest, RejectsNonFiniteCoefficients) {
    const double nanVal = std::numeric_limits<double>::quiet_NaN();
    EXPECT_THROW(TanhCubicNonlinearModel(nanVal, 1.0), std::invalid_argument);
    EXPECT_THROW(TanhCubicNonlinearModel(1.0, nanVal), std::invalid_argument);
}

struct QuadraticDecayCase {
    double a;
    double y0;
    double dt;
    double expected;
};

class QuadraticDecayStepSuite : public ::testing::TestWithParam<QuadraticDecayCase> {};

TEST_P(QuadraticDecayStepSuite, ComputesSingleEulerStep) {
    const auto& tc = GetParam();
    const QuadraticSelfDecayModel model(tc.a);
    EXPECT_NEAR(model.computeStep({tc.y0, 0.0, 0.0, 0.0, tc.dt}), tc.expected, 1e-12);
}

INSTANTIATE_TEST_SUITE_P(Variant16Cases, QuadraticDecayStepSuite, ::testing::Values(
    QuadraticDecayCase{2.0, 0.0, 0.1, 0.0},
    QuadraticDecayCase{1.5, 2.0, 0.1, 1.4},
    QuadraticDecayCase{0.5, -4.0, 0.2, -5.6}
));

class QuadraticDecayConvergenceSuite : public ::testing::TestWithParam<int> {};

TEST_P(QuadraticDecayConvergenceSuite, HalvingStepHalvesGlobalError) {
    const double a = 1.0;
    const double y0 = 1.0;
    const double analyticalAtOne = y0 / (1.0 + a * y0 * 1.0);
    const QuadraticSelfDecayModel model(a);

    const auto measureError = [&](int steps) {
        const double dt = 1.0 / steps;
        const auto trace = model.execute(steps, y0, dt, [](int) { return 0.0; });
        return std::abs(analyticalAtOne - trace.back().yVal);
    };

    const int steps = GetParam();
    const double errCoarse = measureError(steps);
    const double errFine = measureError(steps * 2);
    EXPECT_NEAR(errCoarse / errFine, 2.0, 0.1);
}

INSTANTIATE_TEST_SUITE_P(Variant16Cases, QuadraticDecayConvergenceSuite, ::testing::Values(32, 64, 128));

TEST(QuadraticSelfDecayModelTest, RejectsNonFiniteCoefficient) {
    EXPECT_THROW(QuadraticSelfDecayModel(std::numeric_limits<double>::infinity()),
                 std::invalid_argument);
}

TEST(AbstractDynamicModelTest, ValidatesExecutionConstraintsAndOverflow) {
    const QuadraticSelfDecayModel model(1.0);
    const auto zeroInput = [](int) { return 0.0; };
    const double inf = std::numeric_limits<double>::infinity();

    EXPECT_THROW(model.execute(-1, 1.0, 0.1, zeroInput), std::invalid_argument);
    EXPECT_THROW(model.execute(3, inf, 0.1, zeroInput), std::invalid_argument);
    EXPECT_THROW(model.execute(3, 1.0, inf, zeroInput), std::invalid_argument);
    EXPECT_THROW(model.execute(3, 1.0, 0.0, zeroInput), std::invalid_argument);
    EXPECT_THROW(model.execute(3, 1.0, 0.1, [inf](int) { return inf; }), ModelExecutionError);

    const ThirdOrderLinearModel diverging(2.0, 0.0, 0.0, 0.0);
    EXPECT_THROW(diverging.execute(1, std::numeric_limits<double>::max(), 1.0, zeroInput),
                 ModelExecutionError);

    const auto zeroSteps = model.execute(0, 2.75, 0.5, createInputFn(InputMode::Impulse));
    ASSERT_EQ(zeroSteps.size(), 1u);
    EXPECT_EQ(zeroSteps.front().step, 0);
    EXPECT_DOUBLE_EQ(zeroSteps.front().uVal, 1.0);
    EXPECT_DOUBLE_EQ(zeroSteps.front().yVal, 2.75);
}

TEST(AbstractDynamicModelTest, IdentifiesVariant16ModelLabels) {
    const std::unique_ptr<AbstractDynamicModel> m1 = std::make_unique<ThirdOrderLinearModel>(0.3, 0.2, 0.1, 1.0);
    const std::unique_ptr<AbstractDynamicModel> m2 = std::make_unique<TanhCubicNonlinearModel>(0.5, 1.0);
    const std::unique_ptr<AbstractDynamicModel> m3 = std::make_unique<QuadraticSelfDecayModel>(1.0);
    EXPECT_EQ(m1->label().substr(0, 9), "Model 1.6");
    EXPECT_EQ(m2->label().substr(0, 10), "Model 2.10");
    EXPECT_EQ(m3->label().substr(0, 9), "Model 3.4");
}

} // namespace
} // namespace var16
