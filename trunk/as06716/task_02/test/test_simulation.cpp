#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <cmath>
#include <string>
#include <vector>

#include "models.hpp"
#include "signals.hpp"
#include "simulator.hpp"

using ::testing::DoubleNear;
using ::testing::InSequence;
using ::testing::Return;

namespace {

class MockSignal : public InputSignal {
public:
    MOCK_METHOD(double, value, (int tau), (const, override));
    MOCK_METHOD(std::string, name, (), (const, override));
};

class MockModel : public Model {
public:
    MOCK_METHOD(double, step, (double u), (override));
    MOCK_METHOD(double, output, (), (const, override));
    MOCK_METHOD(std::string, name, (), (const, override));
};

}  // namespace



struct LinearCase {
    double a1;
    double a2;
    double a3;
    double b;
};

class LinearConvergence : public ::testing::TestWithParam<LinearCase> {};

TEST_P(LinearConvergence, StepResponseSettlesAtStaticGain) {
    const auto [a1, a2, a3, b] = GetParam();
    LinearModel16 model(a1, a2, a3, b);

    const auto samples = simulate(model, StepSignal(), 400);

    EXPECT_NEAR(samples.back().y, b / (1.0 - a1 - a2 - a3), 1e-6);
}

TEST_P(LinearConvergence, ImpulseResponseDecaysToZero) {
    const auto [a1, a2, a3, b] = GetParam();
    LinearModel16 model(a1, a2, a3, b);

    const auto samples = simulate(model, ImpulseSignal(), 400);

    EXPECT_NEAR(samples.back().y, 0.0, 1e-6);
}

INSTANTIATE_TEST_SUITE_P(StableSets, LinearConvergence,
                         ::testing::Values(LinearCase{0.5, 0.2, 0.1, 1.0},
                                           LinearCase{0.2, 0.1, 0.1, 2.0},
                                           LinearCase{-0.3, 0.2, 0.1, 1.0}),
                         [](const auto& info) { return "Set" + std::to_string(info.index); });



struct ModeCase {
    int choice;
    std::vector<double> expected;  // u(0), u(1), u(2)
};

class ControlModes : public ::testing::TestWithParam<ModeCase> {};

TEST_P(ControlModes, ModelReceivesExpectedInputs) {
    const auto& [choice, expected] = GetParam();
    const auto signal = makeSignal(choice);
    MockModel model;
    {
        InSequence order;
        for (const double u : expected) {
            EXPECT_CALL(model, step(DoubleNear(u, 1e-12))).WillOnce(Return(0.0));
        }
    }

    simulate(model, *signal, static_cast<int>(expected.size()));
}

INSTANTIATE_TEST_SUITE_P(Signals, ControlModes,
                         ::testing::Values(ModeCase{1, {1.0, 1.0, 1.0}},
                                           ModeCase{2, {1.0, 0.0, 0.0}},
                                           ModeCase{3, {0.0, std::sin(1.0), std::sin(2.0)}}),
                         [](const auto& info) { return "Mode" + std::to_string(info.param.choice); });



TEST(Simulation, MockSignalDrivesTheModel) {
    MockSignal signal;
    {
        InSequence order;
        EXPECT_CALL(signal, value(0)).WillOnce(Return(1.0));
        EXPECT_CALL(signal, value(1)).WillOnce(Return(-2.0));
        EXPECT_CALL(signal, value(2)).WillOnce(Return(0.5));
    }
    LinearModel16 model(0.0, 0.0, 0.0, 1.0);   // y = u

    const auto samples = simulate(model, signal, 3);

    ASSERT_EQ(samples.size(), 3U);
    EXPECT_DOUBLE_EQ(samples[0].y, 1.0);
    EXPECT_DOUBLE_EQ(samples[1].y, -2.0);
    EXPECT_DOUBLE_EQ(samples[2].y, 0.5);
}

TEST(Simulation, ZeroStepsGiveEmptyResult) {
    LinearModel16 model(0.5, 0.2, 0.1, 1.0);

    EXPECT_TRUE(simulate(model, StepSignal(), 0).empty());
}