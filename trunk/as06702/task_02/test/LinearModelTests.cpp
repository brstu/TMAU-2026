#include "LinearModel13.h"
#include "gtest/gtest.h"

#include <tuple>
#include <vector>

class LinearModelStepTest : public testing::TestWithParam<
    std::tuple<double, double, double, double, double, double, double>> {};

TEST_P(LinearModelStepTest, CalculatesNextValueForDifferentParameters) {
    const auto [a1, a2, b, y, previousY, u, expected] = GetParam();
    LinearModel13 model(a1, a2, b, y, previousY);
    EXPECT_NEAR(model.step(u, 0.1), expected, 1e-12);
}

INSTANTIATE_TEST_SUITE_P(
    Variant13Cases,
    LinearModelStepTest,
    testing::Values(
        std::make_tuple(1.2, -0.32, 0.5, 0.0, 0.0, 1.0, 0.5),
        std::make_tuple(0.5, 0.25, 2.0, 1.0, 0.0, 0.0, 0.5),
        std::make_tuple(0.5, 0.25, 2.0, 1.0, 2.0, 1.0, 3.0),
        std::make_tuple(-0.5, 0.1, 1.5, 2.0, 1.0, -2.0, -3.9)
    ));


struct LinearSequenceCase {
    double a1;
    double a2;
    double b;
    double initialY;
    double initialPreviousY;
    std::vector<double> inputs;
    std::vector<double> expected;
};

class LinearModelSequenceTest : public testing::TestWithParam<LinearSequenceCase> {};

TEST_P(LinearModelSequenceTest, ProducesExpectedSequenceAtEverySimulationStep) {
    const LinearSequenceCase testCase = GetParam();
    ASSERT_EQ(testCase.inputs.size(), testCase.expected.size());

    LinearModel13 model(testCase.a1, testCase.a2, testCase.b,
                        testCase.initialY, testCase.initialPreviousY);
    for (std::size_t i = 0; i < testCase.inputs.size(); ++i) {
        SCOPED_TRACE(testing::Message() << "Simulation step " << i + 1);
        EXPECT_NEAR(model.step(testCase.inputs[i], 0.1),
                    testCase.expected[i], 1e-12);
    }
}

INSTANTIATE_TEST_SUITE_P(
    ConvergenceSequences,
    LinearModelSequenceTest,
    testing::Values(
        LinearSequenceCase{0.5, 0.0, 0.0, 8.0, 0.0,
                           {0.0, 0.0, 0.0, 0.0},
                           {4.0, 2.0, 1.0, 0.5}},
        LinearSequenceCase{0.2, 0.1, 0.0, 1.0, 0.0,
                           {0.0, 0.0, 0.0, 0.0},
                           {0.2, 0.14, 0.048, 0.0236}}
    ));

TEST(LinearModelTests, ResetRestoresBothPreviousStates) {
    LinearModel13 model(0.5, 0.25, 2.0, 1.0, 2.0);
    model.step(1.0, 0.1);
    model.reset();
    EXPECT_DOUBLE_EQ(model.value(), 1.0);
    EXPECT_DOUBLE_EQ(model.previousValue(), 2.0);
}
