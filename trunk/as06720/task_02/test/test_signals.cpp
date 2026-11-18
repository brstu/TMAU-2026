#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cmath>
#include <string>

#include "InputSignal.h"
#include "Model.h"
#include "Model2_9.h"
#include "Model3_3.h"
#include "Simulator.h"

using testing::_;
using testing::DoubleEq;
using testing::InSequence;
using testing::Return;

namespace {

    // ===================== Mock-объекты =====================

    // Подменный входной сигнал: значения на каждом шаге задаются в тесте
    class MockInputSignal : public InputSignal {
    public:
        MOCK_METHOD(double, valueAt, (int step), (const, override));
        MOCK_METHOD(std::string, name, (), (const, override));
    };

    // Подменная модель: позволяет проверить, что симулятор передаёт модели нужные u
    class MockModel : public Model {
    public:
        MOCK_METHOD(double, calculateNext, (double u), (override));
        MOCK_METHOD(void, reset, (), (override));
    };

    // ===================== Реальные сигналы из ЛР №1 =====================

    TEST(SignalTest, StepIsConstant) {
        const StepSignal step(2.5);
        EXPECT_DOUBLE_EQ(step.valueAt(0), 2.5);
        EXPECT_DOUBLE_EQ(step.valueAt(7), 2.5);
        EXPECT_EQ(step.name(), "Step");
    }

    TEST(SignalTest, ImpulseIsNonZeroOnlyAtStart) {
        const ImpulseSignal impulse(3.0);
        EXPECT_DOUBLE_EQ(impulse.valueAt(0), 3.0);
        EXPECT_DOUBLE_EQ(impulse.valueAt(1), 0.0);
        EXPECT_DOUBLE_EQ(impulse.valueAt(5), 0.0);
        EXPECT_EQ(impulse.name(), "Impulse");
    }

    TEST(SignalTest, HarmonicFollowsSine) {
        const HarmonicSignal harmonic(2.0);
        EXPECT_DOUBLE_EQ(harmonic.valueAt(0), 0.0);
        EXPECT_NEAR(harmonic.valueAt(1), 2.0 * std::sin(1.0), 1e-12);
        EXPECT_NEAR(harmonic.valueAt(4), 2.0 * std::sin(4.0), 1e-12);
        EXPECT_EQ(harmonic.name(), "Harmonic");
    }

    TEST(SignalTest, DefaultAmplitudeIsOne) {
        EXPECT_DOUBLE_EQ(StepSignal().valueAt(3), 1.0);
        EXPECT_DOUBLE_EQ(ImpulseSignal().valueAt(0), 1.0);
        EXPECT_NEAR(HarmonicSignal().valueAt(1), std::sin(1.0), 1e-12);
    }

    TEST(SignalFactoryTest, MenuChoiceSelectsSignals) {
        ASSERT_EQ(makeSignals(1).size(), 1u);
        EXPECT_EQ(makeSignals(1)[0]->name(), "Step");
        EXPECT_EQ(makeSignals(2)[0]->name(), "Impulse");
        EXPECT_EQ(makeSignals(3)[0]->name(), "Harmonic");

        const auto all = makeSignals(4);
        ASSERT_EQ(all.size(), 3u);
        EXPECT_EQ(all[0]->name(), "Step");
        EXPECT_EQ(all[1]->name(), "Impulse");
        EXPECT_EQ(all[2]->name(), "Harmonic");

        EXPECT_TRUE(makeSignals(0).empty());
    }

    // ===================== Симулятор с подменой входного сигнала =====================

    TEST(SimulatorMockTest, ResetIsCalledBeforeSteps) {
        MockModel model;
        MockInputSignal input;
        {
            InSequence order;
            EXPECT_CALL(model, reset()).Times(1);
            EXPECT_CALL(input, valueAt(0)).WillOnce(Return(1.0));
            EXPECT_CALL(model, calculateNext(DoubleEq(1.0))).WillOnce(Return(10.0));
            EXPECT_CALL(input, valueAt(1)).WillOnce(Return(2.0));
            EXPECT_CALL(model, calculateNext(DoubleEq(2.0))).WillOnce(Return(20.0));
        }

        const auto points = runSimulation(model, input, 2);
        ASSERT_EQ(points.size(), 2u);
        EXPECT_EQ(points[1].step, 1);
        EXPECT_DOUBLE_EQ(points[1].u, 2.0);
        EXPECT_DOUBLE_EQ(points[1].y, 20.0);
    }

    TEST(SimulatorMockTest, ZeroStepsDoNothing) {
        MockModel model;
        MockInputSignal input;
        EXPECT_CALL(model, reset()).Times(0);
        EXPECT_CALL(model, calculateNext(_)).Times(0);
        EXPECT_CALL(input, valueAt(_)).Times(0);
        EXPECT_TRUE(runSimulation(model, input, 0).empty());
        EXPECT_TRUE(runSimulation(model, input, -3).empty());
    }

    TEST(SimulatorMockTest, BaseModelHasNoStabilityCriterion) {
        MockModel model;
        std::string report;
        EXPECT_TRUE(model.Model::isStable(report));
        EXPECT_FALSE(report.empty());
    }

    // ===================== Режимы управления через mock-сигнал =====================
    // Mock-сигнал воспроизводит ступенчатый, импульсный и гармонический режимы,
    // реакция реальной модели сравнивается с ручным расчётом.

    TEST(ControlModeMockTest, StepModeOnModel3_3) {
        MockInputSignal input;
        EXPECT_CALL(input, valueAt(_)).Times(3).WillRepeatedly(Return(1.0));

        Model3_3 model(1.0, 1.0, 0.1);
        const auto points = runSimulation(model, input, 3);
        EXPECT_NEAR(points[0].y, 0.1, 1e-12);
        EXPECT_NEAR(points[1].y, 0.19, 1e-12);
        EXPECT_NEAR(points[2].y, 0.271, 1e-12);
    }

    TEST(ControlModeMockTest, ImpulseModeOnModel2_9) {
        MockInputSignal input;
        EXPECT_CALL(input, valueAt(0)).WillOnce(Return(4.0));
        EXPECT_CALL(input, valueAt(testing::Gt(0))).WillRepeatedly(Return(0.0));

        Model2_9 model(0.5, 1.0);
        const auto points = runSimulation(model, input, 4);
        // y1 = sqrt(4) = 2, далее свободное затухание с множителем a = 0.5
        EXPECT_DOUBLE_EQ(points[0].y, 2.0);
        EXPECT_DOUBLE_EQ(points[1].y, 1.0);
        EXPECT_DOUBLE_EQ(points[2].y, 0.5);
        EXPECT_DOUBLE_EQ(points[3].y, 0.25);
    }

    TEST(ControlModeMockTest, HarmonicModeOnModel2_9) {
        // гармоника с амплитудой 4: u = 4*sin(t), подаётся через mock
        MockInputSignal input;
        EXPECT_CALL(input, valueAt(_)).WillRepeatedly(
            [](int t) { return 4.0 * std::sin(static_cast<double>(t)); });

        Model2_9 model(0.0, 1.0);  // a = 0: выход равен нелинейности от входа
        for (const auto& p : runSimulation(model, input, 6)) {
            const double expected = std::sqrt(std::fabs(p.u)) * (p.u > 0 ? 1.0 : (p.u < 0 ? -1.0 : 0.0));
            EXPECT_NEAR(p.y, expected, 1e-12) << "step " << p.step;
        }
    }

    TEST(ControlModeMockTest, MockAndRealSignalGiveSameResult) {
        MockInputSignal mock;
        EXPECT_CALL(mock, valueAt(_)).WillRepeatedly(
            [](int t) { return std::sin(static_cast<double>(t)); });

        Model3_3 m1(1.0, 1.0, 0.1);
        Model3_3 m2(1.0, 1.0, 0.1);
        const auto withMock = runSimulation(m1, mock, 20);
        const auto withReal = runSimulation(m2, HarmonicSignal(), 20);
        for (size_t i = 0; i < withMock.size(); ++i) {
            EXPECT_DOUBLE_EQ(withMock[i].y, withReal[i].y);
        }
    }

} // namespace
