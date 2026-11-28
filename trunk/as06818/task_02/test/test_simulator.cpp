#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cmath>
#include <vector>

#include "../src/input_signal.hpp"
#include "../src/simulator.hpp"
#include "mock_model.hpp"

using ::testing::DoubleEq;
using ::testing::InSequence;
using ::testing::Return;

// Симулятор сбрасывает модель и делает ровно n шагов.
TEST(SimulatorTest, CallsResetOnceAndStepNTimes) {
    MockModel mock;
    EXPECT_CALL(mock, reset()).Times(1);
    EXPECT_CALL(mock, step(DoubleEq(1.0))).Times(5).WillRepeatedly(Return(0.5));

    const std::vector<SimulationPoint> points =
        simulate(mock, SignalType::Step, 5);

    ASSERT_EQ(points.size(), 5U);
}

// Подмена входного сигнала: ступенчатому входу соответствуют единицы.
TEST(SimulatorTest, FeedsStepInput) {
    MockModel mock;
    EXPECT_CALL(mock, reset());
    EXPECT_CALL(mock, step(DoubleEq(1.0))).Times(3).WillRepeatedly(Return(0.0));

    const std::vector<SimulationPoint> points =
        simulate(mock, SignalType::Step, 3);

    for (const SimulationPoint& point : points) {
        EXPECT_DOUBLE_EQ(point.u, 1.0);
    }
}

// Подмена входного сигнала: импульс подаётся только на первом шаге,
// затем нули (проверяем точную последовательность аргументов step).
TEST(SimulatorTest, FeedsImpulseInputInOrder) {
    MockModel mock;
    EXPECT_CALL(mock, reset());

    {
        InSequence seq;
        EXPECT_CALL(mock, step(DoubleEq(1.0))).WillOnce(Return(0.0));
        EXPECT_CALL(mock, step(DoubleEq(0.0))).Times(3)
            .WillRepeatedly(Return(0.0));
    }

    const std::vector<SimulationPoint> points =
        simulate(mock, SignalType::Impulse, 4);

    ASSERT_EQ(points.size(), 4U);
    EXPECT_DOUBLE_EQ(points[0].u, 1.0);
    EXPECT_DOUBLE_EQ(points[1].u, 0.0);
}

// Подмена входного сигнала: гармонический вход u_tau = sin(tau).
TEST(SimulatorTest, FeedsHarmonicInputInOrder) {
    MockModel mock;
    EXPECT_CALL(mock, reset());

    {
        InSequence seq;
        EXPECT_CALL(mock, step(DoubleEq(0.0))).WillOnce(Return(0.0));
        EXPECT_CALL(mock, step(DoubleEq(std::sin(1.0)))).WillOnce(Return(0.0));
        EXPECT_CALL(mock, step(DoubleEq(std::sin(2.0)))).WillOnce(Return(0.0));
    }

    const std::vector<SimulationPoint> points =
        simulate(mock, SignalType::Harmonic, 3);

    for (std::size_t i = 0; i < points.size(); ++i) {
        EXPECT_EQ(points[i].tau, static_cast<int>(i));
    }
}

// Симулятор собирает траекторию: номера шагов по порядку, y — значения,
// возвращённые моделью.
TEST(SimulatorTest, CollectsTrajectory) {
    MockModel mock;
    EXPECT_CALL(mock, reset());
    EXPECT_CALL(mock, step(DoubleEq(1.0))).Times(3)
        .WillOnce(Return(1.0))
        .WillOnce(Return(2.0))
        .WillOnce(Return(3.0));

    const std::vector<SimulationPoint> points =
        simulate(mock, SignalType::Step, 3);

    ASSERT_EQ(points.size(), 3U);
    EXPECT_EQ(points[0].tau, 0);
    EXPECT_EQ(points[2].tau, 2);
    EXPECT_DOUBLE_EQ(points[0].y, 1.0);
    EXPECT_DOUBLE_EQ(points[1].y, 2.0);
    EXPECT_DOUBLE_EQ(points[2].y, 3.0);
}
