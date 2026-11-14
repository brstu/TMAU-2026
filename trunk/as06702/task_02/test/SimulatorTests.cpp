#include "InputSignal.h"
#include "LinearModel13.h"
#include "Simulator.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <memory>
#include <tuple>
#include <vector>

class SimulatorSignalTest : public testing::TestWithParam<std::tuple<InputKind, double, double>> {};

TEST_P(SimulatorSignalTest, CalculatesTheFirstStepForSelectedInput) {
    const auto [kind, expectedU, expectedY] = GetParam();
    std::unique_ptr<InputSignal> signal = createInputSignal(kind);
    LinearModel13 model(0.0, 0.0, 2.0);

    const std::vector<ResultRow> rows = Simulator::run(model, *signal, 1, 0.1);
    ASSERT_EQ(rows.size(), 1u);
    EXPECT_DOUBLE_EQ(rows[0].u, expectedU);
    EXPECT_DOUBLE_EQ(rows[0].y, expectedY);
}

INSTANTIATE_TEST_SUITE_P(
    AllInputModes,
    SimulatorSignalTest,
    testing::Values(
        std::make_tuple(InputKind::Step, 1.0, 2.0),
        std::make_tuple(InputKind::Impulse, 1.0, 2.0),
        std::make_tuple(InputKind::Harmonic, 0.0, 0.0)
    ));

class MockInputSignal : public InputSignal {
public:
    MOCK_METHOD(double, valueAt, (int tau), (const, override));
    MOCK_METHOD(std::string, name, (), (const, override));
};

TEST(SimulatorMockTests, UsesTheValuesProvidedByMockInputSignal) {
    MockInputSignal signal;
    EXPECT_CALL(signal, name()).WillRepeatedly(testing::Return(std::string("Mock")));
    EXPECT_CALL(signal, valueAt(0)).WillOnce(testing::Return(1.0));
    EXPECT_CALL(signal, valueAt(1)).WillOnce(testing::Return(0.0));
    EXPECT_CALL(signal, valueAt(2)).WillOnce(testing::Return(-1.0));

    LinearModel13 model(0.0, 0.0, 2.0);
    const std::vector<ResultRow> rows = Simulator::run(model, signal, 3, 0.1);

    ASSERT_EQ(rows.size(), 3u);
    EXPECT_DOUBLE_EQ(rows[0].u, 1.0);
    EXPECT_DOUBLE_EQ(rows[1].u, 0.0);
    EXPECT_DOUBLE_EQ(rows[2].u, -1.0);
    EXPECT_DOUBLE_EQ(rows[0].y, 2.0);
    EXPECT_DOUBLE_EQ(rows[1].y, 0.0);
    EXPECT_DOUBLE_EQ(rows[2].y, -2.0);
}

TEST(SimulatorTests, RejectsZeroNumberOfSteps) {
    StepInput signal;
    LinearModel13 model(0.5, 0.2, 1.0);
    EXPECT_THROW(Simulator::run(model, signal, 0, 0.1), std::invalid_argument);
}
