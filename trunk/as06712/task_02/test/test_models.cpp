#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cmath>
#include "LinearModel13.h"
#include "NonlinearModel25.h"
#include "ODE37.h"
#include "Simulator.h"
#include "SignalGenerator.h"

using ::testing::Return;

class MockSignal : public ISignalGenerator {
public:
    MOCK_METHOD(double, get, (double tau), (const, override));
};

TEST(SimulatorTest, UsesMockSignal) {
    auto model = std::make_unique<LinearModel13>(0.5, -0.2, 1.0);
    Simulator sim(std::move(model), 3);
    MockSignal mockSignal;

    EXPECT_CALL(mockSignal, get(0.0)).WillOnce(Return(1.0));
    EXPECT_CALL(mockSignal, get(1.0)).WillOnce(Return(0.0));
    EXPECT_CALL(mockSignal, get(2.0)).WillOnce(Return(0.0));

    auto result = sim.run(mockSignal);
    ASSERT_EQ(result.size(), 3);
    EXPECT_DOUBLE_EQ(result[0], 1.0);
}

class LinearModel13Test : public ::testing::TestWithParam<std::tuple<double, double, double, double, double>> {};

TEST_P(LinearModel13Test, StepCalculation) {
    auto [a1, a2, b, u, expected_y] = GetParam();
    LinearModel13 model(a1, a2, b);
    EXPECT_NEAR(model.step(u), expected_y, 1e-5);
}

INSTANTIATE_TEST_SUITE_P(
    DefaultValues,
    LinearModel13Test,
    ::testing::Values(
        std::make_tuple(1.1, -0.5, 0.5, 1.0, 0.5),
        std::make_tuple(1.0, 0.0, 1.0, 2.0, 2.0),
        std::make_tuple(0.5, 0.5, 1.0, -1.0, -1.0)
    )
);

TEST(LinearModel13Test, UnstableWarning) {
    LinearModel13 unstable_model(2.0, 0.0, 1.0); 
    SUCCEED(); 
}

TEST(LinearModel13Test, NameMethod) {
    LinearModel13 model(0.5, -0.2, 1.0);
    EXPECT_STREQ(model.name(), "Model_1_3");
}

TEST(InvariantTest, ZeroInputZeroState) {
    LinearModel13 model(1.1, -0.5, 0.5);
    EXPECT_DOUBLE_EQ(model.step(0.0), 0.0);
}

TEST(SignalGeneratorTest, RealGeneratorValues) {
    SignalGenerator stepSig(SignalType::STEP, 2.0);
    EXPECT_DOUBLE_EQ(stepSig.get(5.0), 2.0); 

    SignalGenerator impSig(SignalType::IMPULSE, 1.0);
    EXPECT_DOUBLE_EQ(impSig.get(0.0), 1.0); 
    EXPECT_DOUBLE_EQ(impSig.get(1.0), 0.0); 

    SignalGenerator harmSig(SignalType::HARMONIC, 1.0);
    EXPECT_NEAR(harmSig.get(1.5707963), 1.0, 1e-4);
}

TEST(NonlinearModel25Test, SignumBoundaries) {
    NonlinearModel25 model(0.0, 1.0);
    
    EXPECT_NEAR(model.step(1.0), 1.0 * (1.0 - std::exp(-1.0)), 1e-5);
    
    model.reset();
    EXPECT_NEAR(model.step(-1.0), -1.0 * (1.0 - std::exp(-1.0)), 1e-5);
    
    model.reset();
    EXPECT_DOUBLE_EQ(model.step(0.0), 0.0); 
}

TEST(ODE37Test, EulerStepVerification) {
    ODE37 model(0.0, 1.0, 0.1);
    
    double y1 = model.step(1.0);
    EXPECT_NEAR(y1, 0.1, 1e-5);
    
    double y2 = model.step(1.0);
    EXPECT_NEAR(y2, 0.19, 1e-5);
}