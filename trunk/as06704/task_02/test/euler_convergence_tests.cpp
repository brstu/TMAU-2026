#include "input_signal.h"
#include "model.h"
#include "simulator.h"

#include <gtest/gtest.h>

#include <cmath>
#include <cstddef>

namespace {

struct EulerCase {
    const char* name;
    double dt;
    std::size_t steps;
    double maximumAllowedError;
};

class EulerConvergenceTest : public testing::TestWithParam<EulerCase> {};

TEST_P(EulerConvergenceTest, EulerResultApproachesAnalyticalSolution) {
    const EulerCase& testCase = GetParam();

    constexpr double a = 0.5;
    constexpr double b = 0.2;
    constexpr double input = 1.0;
    constexpr double initialY = 0.0;
    constexpr double totalTime = 1.0;

    const DifferentialModel310 model(a, b);
    const StepSignal signal;
    const Simulator simulator;

    const auto samples = simulator.run(
        model, signal, testCase.steps, initialY, testCase.dt
    );

    ASSERT_EQ(samples.size(), testCase.steps + 1);

    const double forcing = b + input;
    const double exact = forcing / a
        + (initialY - forcing / a) * std::exp(-a * totalTime);
    const double error = std::abs(samples.back().output - exact);

    EXPECT_LT(error, testCase.maximumAllowedError)
        << "dt = " << testCase.dt;
}

INSTANTIATE_TEST_SUITE_P(
    EulerStepSizes,
    EulerConvergenceTest,
    testing::Values(
        EulerCase{"DT_020", 0.2, 5, 0.04},
        EulerCase{"DT_010", 0.1, 10, 0.02},
        EulerCase{"DT_005", 0.05, 20, 0.01}
    ),
    [](const testing::TestParamInfo<EulerCase>& info) {
        return info.param.name;
    }
);

} // namespace
