#include <gtest/gtest.h>

#include <cmath>
#include <vector>

#include "Model21.h"

namespace {

struct Model21Case {
    double a;
    double b;
    double c;
    double d;
    std::vector<double> inputs;
    std::vector<double> expected;
};

} // namespace

class Model21SequenceTest : public ::testing::TestWithParam<Model21Case> {
};

TEST_P(Model21SequenceTest, MatchesHandCalculatedSequence) {
    const Model21Case& testCase = GetParam();
    Model21 model(testCase.a, testCase.b, testCase.c, testCase.d);

    ASSERT_EQ(testCase.inputs.size(), testCase.expected.size());

    for (std::size_t i = 0; i < testCase.inputs.size(); ++i) {
        EXPECT_NEAR(model.step(testCase.inputs[i]), testCase.expected[i], 1e-12)
            << "step " << i;
    }
}

INSTANTIATE_TEST_SUITE_P(
    KnownCoefficients,
    Model21SequenceTest,
    ::testing::Values(
        Model21Case{0.5, 0.1, 1.0, 0.2, {1.0, 1.0, 1.0},
                    {1.0,
                     1.5 + 0.2 * std::sin(1.0),
                     0.5 * (1.5 + 0.2 * std::sin(1.0)) - 0.1 + 1.0
                         + 0.2 * std::sin(1.0)}},
        Model21Case{0.0, 0.5, 2.0, 1.0, {0.0, 2.0, -1.0},
                    {0.0, 4.0, -2.0 + std::sin(2.0)}},
        Model21Case{0.5, 0.1, 1.0, 0.2, {1.0, 0.0, 1.0},
                    {1.0,
                     0.5 + 0.2 * std::sin(1.0),
                     0.5 * (0.5 + 0.2 * std::sin(1.0)) - 0.1 + 1.0}}));

TEST(Model21Test, ZeroInputAndZeroStateKeepSystemAtRest) {
    Model21 model(0.5, 0.05, 0.8, 0.2);

    EXPECT_DOUBLE_EQ(model.step(0.0), 0.0);
    EXPECT_DOUBLE_EQ(model.step(0.0), 0.0);
    EXPECT_DOUBLE_EQ(model.step(0.0), 0.0);
}

TEST(Model21Test, ResetClearsState) {
    Model21 model(0.5, 0.1, 1.0, 0.2);
    model.step(1.0);
    model.step(2.0);

    model.reset();

    Model21 fresh(0.5, 0.1, 1.0, 0.2);
    EXPECT_DOUBLE_EQ(model.step(1.0), fresh.step(1.0));
}

TEST(Model21Test, QuadraticTermUsesPreviousOutput) {
    Model21 model(1.0, 0.25, 0.0, 0.0, 2.0);

    EXPECT_DOUBLE_EQ(model.step(0.0), 1.0);
    EXPECT_DOUBLE_EQ(model.step(0.0), 0.0);
}

TEST(Model21Test, InitialStateIsUsedAfterReset) {
    Model21 model(0.5, 0.1, 1.0, 0.2, 3.0);

    EXPECT_DOUBLE_EQ(model.output(), 3.0);

    model.reset();
    EXPECT_DOUBLE_EQ(model.output(), 3.0);
}

TEST(Model21Test, ExposesNameAndNonLinearity) {
    const Model21 model(0.5, 0.05, 0.8, 0.2);

    EXPECT_EQ(model.name(), "Model 2.1");
    EXPECT_FALSE(model.isLinear());
    EXPECT_TRUE(model.characteristicPolynomial().empty());
}
