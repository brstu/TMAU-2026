#include "../../task_01/src/model_realizations.hpp"
#include "../src/stability_analyzer.hpp"
#include <gtest/gtest.h>
#include <cmath>
#include <limits>
#include <tuple>
#include <vector>

TEST(LinearModelTest, KnownManualValues) {
    LinearModel m(0.5, -0.25, 2.0, 1.0);
    EXPECT_DOUBLE_EQ(m.next_step(4.0, 2.0, 3.0, 1.0), 8.5);
}
TEST(LinearModelTest, ZeroInputAndZeroStateRemainAtRest) {
    LinearModel m(0.7, 0.2, 1.0, 0.5);
    EXPECT_DOUBLE_EQ(m.next_step(0,0,0,0), 0);
}
class LinearStepParameterized : public ::testing::TestWithParam<std::tuple<double,double,double,double,double,double,double,double>> {};
TEST_P(LinearStepParameterized, ComputesStepForMultipleDatasets) {
    const auto [a1,a2,b1,b2,y,yPrev,u,uPrev] = GetParam();
    LinearModel m(a1,a2,b1,b2);
    EXPECT_NEAR(m.next_step(y,yPrev,u,uPrev),a1*y+a2*yPrev+b1*u+b2*uPrev,1e-12);
}
INSTANTIATE_TEST_SUITE_P(Inputs, LinearStepParameterized, ::testing::Values(
    std::make_tuple(0.5,0.2,1.0,0.0,2.0,1.0,1.0,0.0),
    std::make_tuple(-0.3,0.1,0.4,0.2,1.5,-2.0,3.0,1.0),
    std::make_tuple(0.0,0.0,2.0,-1.0,0.0,0.0,2.0,4.0)));

TEST(SaturationTest, ClipsAtUpperAndLowerLimitsAndPassesInterior) {
    NoneLinearModel m(0.5,2.0,-3.0,4.0);
    EXPECT_DOUBLE_EQ(m.next_step(0,0,4.0,0),8.0);
    EXPECT_DOUBLE_EQ(m.next_step(0,0,-3.0,0),-6.0);
    EXPECT_DOUBLE_EQ(m.next_step(0,0,8.0,0),8.0);
    EXPECT_DOUBLE_EQ(m.next_step(0,0,-8.0,0),-6.0);
    EXPECT_DOUBLE_EQ(m.next_step(0,0,1.25,0),2.5);
}
TEST(DifferentialModelTest, EulerStepMatchesFormula) {
    SimpleDifferentialModel m(-0.2,3.0,0.1);
    const double y=2.0,u=1.5;
    EXPECT_NEAR(m.next_step(y,0,u,0), y+0.1*(-0.2*y*y*y+3.0*u),1e-12);
}

TEST(StabilityTest, FirstOrderSpecialCaseStableWhenAbsA1BelowOne) {
    EXPECT_TRUE(StabilityAnalyzer::is_asymptotically_stable(0.8,0.0));
}
TEST(StabilityTest, UnstableWhenRootOutsideUnitCircle) {
    EXPECT_FALSE(StabilityAnalyzer::is_asymptotically_stable(1.2,0.0));
}
TEST(StabilityTest, UnitCircleBoundaryIsNotAsymptoticallyStable) {
    EXPECT_FALSE(StabilityAnalyzer::is_asymptotically_stable(1.0,0.0));
    EXPECT_FALSE(StabilityAnalyzer::is_asymptotically_stable(-1.0,0.0));
}
TEST(StabilityTest, SecondOrderStableExampleAndRoots) {
    auto r=StabilityAnalyzer::roots(0.3,0.1); // z^2 - 0.3z - 0.1 = 0; roots 0.5 and -0.2
    EXPECT_NEAR(r.first.real(),0.5,1e-12);
    EXPECT_NEAR(r.second.real(),-0.2,1e-12);
    EXPECT_TRUE(StabilityAnalyzer::is_asymptotically_stable(0.3,0.1));
}
TEST(StabilityTest, RejectsNonFiniteCoefficients) {
    EXPECT_FALSE(StabilityAnalyzer::is_asymptotically_stable(
        std::numeric_limits<double>::infinity(),0.0));
}
