#include <gtest/gtest.h>

#include <vector>

#include "Model17.h"

namespace {

struct Model17Case {
    double a;
    double b1;
    double b2;
    double b3;
    std::vector<double> inputs;
    std::vector<double> expected;
};

} // namespace

class Model17SequenceTest : public ::testing::TestWithParam<Model17Case> {
};

TEST_P(Model17SequenceTest, MatchesHandCalculatedSequence) {
    const Model17Case& testCase = GetParam();
    Model17 model(testCase.a, testCase.b1, testCase.b2, testCase.b3);

    ASSERT_EQ(testCase.inputs.size(), testCase.expected.size());

    for (std::size_t i = 0; i < testCase.inputs.size(); ++i) {
        EXPECT_NEAR(model.step(testCase.inputs[i]), testCase.expected[i], 1e-12)
            << "step " << i;
    }
}

INSTANTIATE_TEST_SUITE_P(
    KnownCoefficients,
    Model17SequenceTest,
    ::testing::Values(
        Model17Case{0.5, 1.0, 0.5, 0.25, {1.0, 1.0, 1.0}, {1.0, 2.0, 2.75}},
        Model17Case{0.0, 1.0, -1.0, 0.5, {2.0, -1.0, 3.0, 0.0},
                    {2.0, -3.0, 5.0, -3.5}},
        Model17Case{0.9, 0.7, 0.2, 0.1, {1.0, 0.0, 0.0, 0.0},
                    {0.7, 0.83, 0.847, 0.7623}}));

TEST(Model17Test, ZeroInputAndZeroStateKeepSystemAtRest) {
    Model17 model(0.7, 0.5, 0.2, 0.1);

    EXPECT_DOUBLE_EQ(model.step(0.0), 0.0);
    EXPECT_DOUBLE_EQ(model.step(0.0), 0.0);
    EXPECT_DOUBLE_EQ(model.step(0.0), 0.0);
}

TEST(Model17Test, ResetClearsInputHistory) {
    Model17 model(0.7, 0.5, 0.2, 0.1);
    model.step(1.0);
    model.step(2.0);
    model.step(3.0);

    model.reset();

    Model17 fresh(0.7, 0.5, 0.2, 0.1);
    EXPECT_DOUBLE_EQ(model.step(4.0), fresh.step(4.0));
}

TEST(Model17Test, InitialStateIsUsedAfterReset) {
    Model17 model(0.5, 1.0, 0.0, 0.0, 4.0);

    EXPECT_DOUBLE_EQ(model.output(), 4.0);
    EXPECT_DOUBLE_EQ(model.step(0.0), 2.0);

    model.reset();
    EXPECT_DOUBLE_EQ(model.output(), 4.0);
}

TEST(Model17Test, CharacteristicPolynomialIsZMinusA) {
    const Model17 model(0.7, 0.5, 0.2, 0.1);
    const std::vector<double> polynomial = model.characteristicPolynomial();

    ASSERT_EQ(polynomial.size(), 2u);
    EXPECT_DOUBLE_EQ(polynomial[0], 1.0);
    EXPECT_DOUBLE_EQ(polynomial[1], -0.7);
}

TEST(Model17Test, ExposesNameAndLinearity) {
    const Model17 model(0.7, 0.5, 0.2, 0.1);

    EXPECT_EQ(model.name(), "Model 1.7");
    EXPECT_TRUE(model.isLinear());
}
