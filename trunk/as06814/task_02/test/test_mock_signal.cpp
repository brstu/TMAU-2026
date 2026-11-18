#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../src/models.h"

class MockSignalProvider : public ISignalProvider {
public:
    MOCK_METHOD(double, getSignal, (int t), (const, override));
};

using ::testing::Return;
using ::testing::_;

TEST(MockSignalTest, StepSignalSimulation) {
    MockSignalProvider mockSignal;
    EXPECT_CALL(mockSignal, getSignal(_))
        .WillRepeatedly(Return(1.0)); // Ступенчатый сигнал = 1.0

    LinearModel1_4 model(0.8, 0.2);
    auto result = model.simulateWithProvider(mockSignal, 3);

    ASSERT_EQ(result.size(), 3u);
    EXPECT_NEAR(result[0], 0.2, 1e-6);
    EXPECT_NEAR(result[1], 0.16, 1e-6);
    EXPECT_NEAR(result[2], 0.128, 1e-6);
}

TEST(MockSignalTest, ImpulseSignalSimulation) {
    MockSignalProvider mockSignal;
    EXPECT_CALL(mockSignal, getSignal(0)).WillOnce(Return(1.0));
    EXPECT_CALL(mockSignal, getSignal(1)).WillOnce(Return(0.0));
    EXPECT_CALL(mockSignal, getSignal(2)).WillOnce(Return(0.0));

    LinearModel1_4 model(0.5, 0.4);
    auto result = model.simulateWithProvider(mockSignal, 3);

    ASSERT_EQ(result.size(), 3u);
    EXPECT_NEAR(result[0], 0.4, 1e-6);
    EXPECT_NEAR(result[1], -0.2, 1e-6);
    EXPECT_NEAR(result[2], -0.1, 1e-6);
}

TEST(MockSignalTest, HarmonicSignalSimulation) {
    MockSignalProvider mockSignal;
    EXPECT_CALL(mockSignal, getSignal(0)).WillOnce(Return(0.0));
    EXPECT_CALL(mockSignal, getSignal(1)).WillOnce(Return(0.84147098)); // sin(1)

    DiffModel3_2 model(1.0, 0.1);
    auto result = model.simulateWithProvider(mockSignal, 2);

    ASSERT_EQ(result.size(), 2u);
    EXPECT_NEAR(result[0], 0.0, 1e-6);
    EXPECT_NEAR(result[1], 0.084147, 1e-5);
}