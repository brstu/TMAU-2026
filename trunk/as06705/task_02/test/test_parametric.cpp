#include "InputSignals.hpp"
#include "Models.hpp"
#include "SimulationRunner.hpp"
#include <cmath>
#include <gtest/gtest.h>
#include <tuple>

class SteadyStateConvergence : public ::testing::TestWithParam<int> {};

TEST_P(SteadyStateConvergence, ConvergesToExpected) {
    const int n = GetParam();
    Runner r(std::make_unique<ThirdOrderLinear>(0.5, 0.0, 0.0, 1.0), n);
    StepSignal s(1.0);
    auto tr = r.execute(s);

    ASSERT_EQ(tr.y.size(), static_cast<size_t>(n));
    if (n >= 60) {
        EXPECT_NEAR(tr.y.back(), 2.0, 1e-12);
    }
}
INSTANTIATE_TEST_SUITE_P(StepCounts, SteadyStateConvergence,
    ::testing::Values(50, 100, 500, 1000));

struct CoeffsCase {
    double a1, a2, a3, b, u, expected_first;
};

class ThirdOrderParams : public ::testing::TestWithParam<CoeffsCase> {};

TEST_P(ThirdOrderParams, FirstOutput) {
    auto c = GetParam();
    ThirdOrderLinear m(c.a1, c.a2, c.a3, c.b);
    EXPECT_NEAR(m.advance(c.u), c.expected_first, 1e-12);
}
INSTANTIATE_TEST_SUITE_P(Combinations, ThirdOrderParams,
    ::testing::Values(
        CoeffsCase{ 0.0, 0.0, 0.0, 1.0, 1.0, 1.0 },
        CoeffsCase{ 0.5, 0.0, 0.0, 1.0, 1.0, 1.0 },
        CoeffsCase{ 1.0, 0.0, 0.0, 1.0, 1.0, 1.0 },
        CoeffsCase{ 0.5, 0.3, 0.2, 2.0, 1.0, 2.0 },
        CoeffsCase{ 0.0, 0.0, 0.0, 0.0, 5.0, 0.0 }
    ));

struct ODECase { double a, b, dt, y0; int steps; };

class ODEEulerConvergence : public ::testing::TestWithParam<ODECase> {};

TEST_P(ODEEulerConvergence, MatchesAnalyticSolution) {
    auto c = GetParam();

    Runner r(std::make_unique<OffsetODE>(c.a, c.b, c.dt, c.y0), c.steps);
    StepSignal s(1.0);
    auto tr = r.execute(s);

    double y = tr.y.back();

    const double t_end = c.steps * c.dt;
    const double y_inf = (c.b + 1.0) / c.a;
    const double y_analytic = y_inf + (c.y0 - y_inf) * std::exp(-c.a * t_end);

    EXPECT_NEAR(y, y_analytic, 1e-2);
}
INSTANTIATE_TEST_SUITE_P(Combinations, ODEEulerConvergence,
    ::testing::Values(
        ODECase{ 1.0, 0.0, 0.01,  1.0, 1000 },
        ODECase{ 2.0, 1.0, 0.005, 0.0, 1000 },
        ODECase{ 0.5, 0.5, 0.01,  2.0, 500 }
    ));

class SignalModelCombination : public ::testing::TestWithParam<int> {};

TEST_P(SignalModelCombination, ProducesFiniteOutput) {
    const int sigChoice = GetParam();
    auto sig = makeSignal(sigChoice, 1.0);
    ASSERT_NE(sig, nullptr);

    Runner r(std::make_unique<ThirdOrderLinear>(0.5, 0.0, 0.0, 1.0), 20);
    auto tr = r.execute(*sig);

    ASSERT_EQ(tr.y.size(), 20u);
    for (double y : tr.y) EXPECT_TRUE(std::isfinite(y));
}
INSTANTIATE_TEST_SUITE_P(SignalTypes, SignalModelCombination,
    ::testing::Values(1, 2, 3));