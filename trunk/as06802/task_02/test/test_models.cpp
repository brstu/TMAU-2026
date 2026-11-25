#include "StabilityAnalyzer.h"

#include <gtest/gtest.h>

#include <cmath>
#include <limits>
#include <memory>
#include <stdexcept>
#include <vector>

namespace var17 {
namespace {

struct MultiStepCase {
    const char* label;
    double a;
    double b1;
    double b2;
    double b3;
    std::vector<double> uSeq;
    std::vector<double> expectedY;
};

class MultiStepTrajectoryTest : public ::testing::TestWithParam<MultiStepCase> {};

TEST_P(MultiStepTrajectoryTest, ReproducesExpectedSequence) {
    const auto& tc = GetParam();
    const MultiStepControlSystem sys(tc.a, tc.b1, tc.b2, tc.b3);
    const int n = static_cast<int>(tc.expectedY.size()) - 1;
    const auto traj = sys.runSimulation(n, tc.expectedY.front(), 1.0,
        [&](int idx) { return tc.uSeq.at(static_cast<std::size_t>(idx)); });

    ASSERT_EQ(traj.size(), tc.expectedY.size());
    for (std::size_t i = 0; i < traj.size(); ++i) {
        SCOPED_TRACE(i);
        EXPECT_EQ(traj[i].tau, static_cast<int>(i));
        EXPECT_DOUBLE_EQ(traj[i].control, tc.uSeq[i]);
        EXPECT_NEAR(traj[i].state, tc.expectedY[i], 1e-12);
    }
}

INSTANTIATE_TEST_SUITE_P(Variant17Models, MultiStepTrajectoryTest, ::testing::Values(
    MultiStepCase{"UnitStep", 0.5, 1.0, 2.0, 3.0, {1, 1, 1, 1, 1}, {0, 1, 3.5, 7.75, 9.875}},
    MultiStepCase{"UnitImpulse", 0.5, 2.0, 4.0, 6.0, {1, 0, 0, 0, 0}, {0, 2, 5, 8.5, 4.25}},
    MultiStepCase{"ZeroEquilibrium", 0.8, 1.5, -0.5, 2.0, {0, 0, 0, 0}, {0, 0, 0, 0}},
    MultiStepCase{"DivergingOscillation", -2.0, 1.0, 1.0, 1.0, {0, 0, 0, 0}, {1, -2, 4, -8}}
), [](const auto& info) { return info.param.label; });

TEST(MultiStepControlSystemTest, ConvergesToAnalyticalSteadyState) {
    const MultiStepControlSystem sys(0.5, 1.0, 2.0, 3.0);
    const auto traj = sys.runSimulation(60, 0.0, 1.0, buildControlGenerator(SignalKind::Step));
    EXPECT_NEAR(traj.back().state, 12.0, 1e-12);
}

TEST(MultiStepControlSystemTest, ValidatesCoefficients) {
    const double inf = std::numeric_limits<double>::infinity();
    EXPECT_THROW(MultiStepControlSystem(inf, 1.0, 1.0, 1.0), std::invalid_argument);
    EXPECT_THROW(MultiStepControlSystem(0.5, inf, 1.0, 1.0), std::invalid_argument);
    EXPECT_THROW(MultiStepControlSystem(0.5, 1.0, inf, 1.0), std::invalid_argument);
    EXPECT_THROW(MultiStepControlSystem(0.5, 1.0, 1.0, inf), std::invalid_argument);
}

struct QuadHarmonicCase {
    const char* label;
    double a;
    double b;
    double c;
    double d;
    std::vector<double> uSeq;
    std::vector<double> expectedY;
};

class QuadraticHarmonicTrajectoryTest : public ::testing::TestWithParam<QuadHarmonicCase> {};

TEST_P(QuadraticHarmonicTrajectoryTest, MatchesNonlinearStepHistory) {
    const auto& tc = GetParam();
    const QuadraticHarmonicSystem sys(tc.a, tc.b, tc.c, tc.d);
    const int n = static_cast<int>(tc.expectedY.size()) - 1;
    const auto traj = sys.runSimulation(n, tc.expectedY.front(), 1.0,
        [&](int idx) { return tc.uSeq.at(static_cast<std::size_t>(idx)); });

    ASSERT_EQ(traj.size(), tc.expectedY.size());
    for (std::size_t i = 0; i < traj.size(); ++i) {
        SCOPED_TRACE(i);
        EXPECT_NEAR(traj[i].state, tc.expectedY[i], 1e-12);
    }
}

INSTANTIATE_TEST_SUITE_P(Variant17Models, QuadraticHarmonicTrajectoryTest, ::testing::Values(
    QuadHarmonicCase{"ZeroRestState", 0.7, 0.2, 1.5, 3.0, {0, 0, 0, 0}, {0, 0, 0, 0}},
    QuadHarmonicCase{"StepWithDelayedSineAndQuadratic", 0.5, 0.25, 2.0, 1.0,
        {1, 1, 1, 1}, {0, 2.0, 3.0 + std::sin(1.0),
        0.5 * (3.0 + std::sin(1.0)) - 1.0 + 2.0 + std::sin(1.0)}}
), [](const auto& info) { return info.param.label; });

TEST(QuadraticHarmonicSystemTest, ValidatesCoefficients) {
    const double nanVal = std::numeric_limits<double>::quiet_NaN();
    EXPECT_THROW(QuadraticHarmonicSystem(nanVal, 1.0, 1.0, 1.0), std::invalid_argument);
    EXPECT_THROW(QuadraticHarmonicSystem(1.0, nanVal, 1.0, 1.0), std::invalid_argument);
    EXPECT_THROW(QuadraticHarmonicSystem(1.0, 1.0, nanVal, 1.0), std::invalid_argument);
    EXPECT_THROW(QuadraticHarmonicSystem(1.0, 1.0, 1.0, nanVal), std::invalid_argument);
}

struct HarmonicDriveCase {
    double b;
    double y0;
    double uConst;
    double dt;
    int steps;
};

class HarmonicDriveEulerTest : public ::testing::TestWithParam<HarmonicDriveCase> {};

TEST_P(HarmonicDriveEulerTest, MatchesExactLinearGrowthForConstantControl) {
    const auto& tc = GetParam();
    const HarmonicDriveEulerSystem sys(tc.b);
    const auto traj = sys.runSimulation(tc.steps, tc.y0, tc.dt, [&](int) { return tc.uConst; });
    const double totalTime = tc.steps * tc.dt;
    const double analytical = tc.y0 + tc.b * std::sin(tc.uConst) * totalTime;
    EXPECT_NEAR(traj.back().state, analytical, 1e-12);
}

INSTANTIATE_TEST_SUITE_P(Variant17Models, HarmonicDriveEulerTest, ::testing::Values(
    HarmonicDriveCase{2.5, 0.0, 0.0, 0.1, 20},
    HarmonicDriveCase{3.0, 1.5, 1.0, 0.05, 40},
    HarmonicDriveCase{-4.0, 10.0, 0.5, 0.025, 80}
));

TEST(HarmonicDriveEulerSystemTest, ValidatesCoefficients) {
    EXPECT_THROW(HarmonicDriveEulerSystem(std::numeric_limits<double>::infinity()),
                 std::invalid_argument);
}

TEST(BaseSystemModelTest, HandlesEdgeCasesAndInvalidInputs) {
    const HarmonicDriveEulerSystem sys(1.0);
    const auto zeroSignal = [](int) { return 0.0; };
    const double inf = std::numeric_limits<double>::infinity();

    EXPECT_THROW(sys.runSimulation(-1, 0.0, 1.0, zeroSignal), std::invalid_argument);
    EXPECT_THROW(sys.runSimulation(3, inf, 1.0, zeroSignal), std::invalid_argument);
    EXPECT_THROW(sys.runSimulation(3, 0.0, inf, zeroSignal), std::invalid_argument);
    EXPECT_THROW(sys.runSimulation(3, 0.0, -0.1, zeroSignal), std::invalid_argument);
    EXPECT_THROW(sys.runSimulation(3, 0.0, 1.0, [inf](int) { return inf; }), std::runtime_error);

    const MultiStepControlSystem exploding(2.0, 0.0, 0.0, 0.0);
    EXPECT_THROW(exploding.runSimulation(1, std::numeric_limits<double>::max(), 1.0, zeroSignal),
                 std::runtime_error);

    const auto singlePoint = sys.runSimulation(0, 4.25, 0.1, buildControlGenerator(SignalKind::Impulse));
    ASSERT_EQ(singlePoint.size(), 1u);
    EXPECT_EQ(singlePoint.front().tau, 0);
    EXPECT_DOUBLE_EQ(singlePoint.front().control, 1.0);
    EXPECT_DOUBLE_EQ(singlePoint.front().state, 4.25);
}

TEST(BaseSystemModelTest, ReportsVariant17ModelTitles) {
    const std::unique_ptr<BaseSystemModel> m1 = std::make_unique<MultiStepControlSystem>(0.5, 1.0, 1.0, 1.0);
    const std::unique_ptr<BaseSystemModel> m2 = std::make_unique<QuadraticHarmonicSystem>(0.5, 0.1, 1.0, 1.0);
    const std::unique_ptr<BaseSystemModel> m3 = std::make_unique<HarmonicDriveEulerSystem>(2.0);
    EXPECT_EQ(m1->title().substr(0, 9), "Model 1.7");
    EXPECT_EQ(m2->title().substr(0, 9), "Model 2.1");
    EXPECT_EQ(m3->title().substr(0, 9), "Model 3.5");
}

} // namespace
} // namespace var17
