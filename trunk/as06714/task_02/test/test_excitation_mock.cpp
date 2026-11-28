// Входные воздействия и прогон модели: реальные сигналы ЛР №1 и их подмена
// mock-объектами Google Mock для ступенчатого, импульсного и гармонического режимов.

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cmath>
#include <string>

#include "Excitation.h"
#include "Model.h"
#include "Model1_7.h"
#include "Model2_9.h"
#include "Model3_1.h"
#include "Runner.h"

using ::testing::_;
using ::testing::AnyNumber;
using ::testing::DoubleNear;
using ::testing::Invoke;
using ::testing::Return;
using ::testing::Sequence;

namespace {

    class FakeExcitation : public Excitation {
    public:
        MOCK_METHOD(double, sample, (int tau), (const, override));
        MOCK_METHOD(std::string, title, (), (const, override));
    };

    class FakePlant : public Model {
    public:
        MOCK_METHOD(double, calculateNext, (double u), (override));
        MOCK_METHOD(void, reset, (), (override));
    };

    // ---------------- Обёртка над сигналами ЛР №1 ----------------

    TEST(LabExcitationTest, StepKeepsAmplitude) {
        const LabExcitation step(InputType::STEP, 2.5);
        EXPECT_DOUBLE_EQ(step.sample(0), 2.5);
        EXPECT_DOUBLE_EQ(step.sample(31), 2.5);
        EXPECT_EQ(step.title(), "step");
        EXPECT_EQ(step.kind(), InputType::STEP);
    }

    TEST(LabExcitationTest, ImpulseOnlyAtZero) {
        const LabExcitation impulse(InputType::IMPULSE, -4.0);
        EXPECT_DOUBLE_EQ(impulse.sample(0), -4.0);
        EXPECT_DOUBLE_EQ(impulse.sample(1), 0.0);
        EXPECT_DOUBLE_EQ(impulse.sample(9), 0.0);
        EXPECT_EQ(impulse.title(), "impulse");
    }

    TEST(LabExcitationTest, HarmonicIsScaledSine) {
        const LabExcitation wave(InputType::HARMONIC, 3.0);
        for (int tau : { 0, 1, 5, 12 }) {
            EXPECT_NEAR(wave.sample(tau), 3.0 * std::sin(tau), 1e-12);
        }
        EXPECT_EQ(wave.title(), "harmonic");
    }

    TEST(LabExcitationTest, DefaultGainAndUnknownKind) {
        EXPECT_DOUBLE_EQ(LabExcitation(InputType::STEP).sample(4), 1.0);
        const LabExcitation odd(static_cast<InputType>(7));
        EXPECT_DOUBLE_EQ(odd.sample(0), 0.0);  // getInputValue возвращает 0 для неизвестного вида
        EXPECT_EQ(odd.title(), "unknown");
    }

    // ---------------- Прогон с mock-моделью ----------------

    TEST(RunnerWithFakes, CallsResetThenAlternatesSampleAndStep) {
        FakePlant plant;
        FakeExcitation source;
        Sequence order;

        EXPECT_CALL(plant, reset()).InSequence(order);
        EXPECT_CALL(source, sample(0)).InSequence(order).WillOnce(Return(0.5));
        EXPECT_CALL(plant, calculateNext(0.5)).InSequence(order).WillOnce(Return(-1.0));
        EXPECT_CALL(source, sample(1)).InSequence(order).WillOnce(Return(0.25));
        EXPECT_CALL(plant, calculateNext(0.25)).InSequence(order).WillOnce(Return(-2.0));

        const Trace trace = simulateSteps(plant, source, 2);
        EXPECT_THAT(trace.input, ::testing::ElementsAre(0.5, 0.25));
        EXPECT_THAT(trace.output, ::testing::ElementsAre(-1.0, -2.0));
    }

    TEST(RunnerWithFakes, NonPositiveHorizonTouchesNothing) {
        FakePlant plant;
        FakeExcitation source;
        EXPECT_CALL(plant, reset()).Times(0);
        EXPECT_CALL(plant, calculateNext(_)).Times(0);
        EXPECT_CALL(source, sample(_)).Times(0);
        EXPECT_TRUE(simulateSteps(plant, source, 0).output.empty());
        EXPECT_TRUE(simulateSteps(plant, source, -5).input.empty());
    }

    TEST(RunnerWithFakes, BaseModelStabilityStub) {
        FakePlant plant;
        std::string note;
        EXPECT_TRUE(plant.Model::isStable(note));
        EXPECT_NE(note.find("not defined"), std::string::npos);
    }

    // ---------------- Режимы управления через mock-воздействие ----------------

    TEST(ControlModes, StepFedByMockIntoModel1_7) {
        FakeExcitation step;
        EXPECT_CALL(step, sample(_)).Times(4).WillRepeatedly(Return(1.0));

        Model1_7 model(0.6, 0.5, 0.3, 0.2);
        const Trace trace = simulateSteps(model, step, 4);
        EXPECT_THAT(trace.output, ::testing::ElementsAre(
            DoubleNear(0.5, 1e-12), DoubleNear(1.1, 1e-12),
            DoubleNear(1.66, 1e-12), DoubleNear(1.996, 1e-12)));
    }

    TEST(ControlModes, ImpulseFedByMockIntoModel2_9) {
        FakeExcitation impulse;
        EXPECT_CALL(impulse, sample(0)).WillOnce(Return(9.0));
        EXPECT_CALL(impulse, sample(::testing::Gt(0))).Times(AnyNumber()).WillRepeatedly(Return(0.0));

        Model2_9 model(0.4, 1.0);
        const Trace trace = simulateSteps(model, impulse, 3);
        EXPECT_THAT(trace.output, ::testing::ElementsAre(
            DoubleNear(3.0, 1e-12), DoubleNear(1.2, 1e-12), DoubleNear(0.48, 1e-12)));
    }

    TEST(ControlModes, HarmonicFedByMockIntoModel1_7) {
        FakeExcitation wave;
        EXPECT_CALL(wave, sample(_)).WillRepeatedly(Invoke([](int tau) { return std::sin(0.5 * tau); }));

        // при a = 0 выход - это КИХ-фильтр от трёх последних отсчётов входа
        Model1_7 model(0.0, 1.0, 2.0, 3.0);
        const Trace trace = simulateSteps(model, wave, 8);
        for (size_t tau = 2; tau < trace.output.size(); ++tau) {
            const double expected = trace.input[tau] + 2.0 * trace.input[tau - 1] + 3.0 * trace.input[tau - 2];
            EXPECT_NEAR(trace.output[tau], expected, 1e-12) << "tau=" << tau;
        }
    }

    TEST(ControlModes, AnyMockInputGivesSameDecayForModel3_1) {
        FakeExcitation wild;
        EXPECT_CALL(wild, sample(_)).WillRepeatedly(Invoke([](int tau) { return tau % 2 ? 1e3 : -1e3; }));

        Model3_1 driven(1.0, 0.1, 1.0);
        Model3_1 undriven(1.0, 0.1, 1.0);
        const Trace trace = simulateSteps(driven, wild, 6);
        for (double y : trace.output) {
            EXPECT_DOUBLE_EQ(y, undriven.calculateNext(0.0));
        }
    }

    TEST(ControlModes, MockReproducesRealHarmonic) {
        FakeExcitation imitation;
        EXPECT_CALL(imitation, sample(_)).WillRepeatedly(Invoke([](int tau) { return std::sin(tau); }));

        Model2_9 viaMock(0.7, 1.0);
        Model2_9 viaReal(0.7, 1.0);
        const Trace a = simulateSteps(viaMock, imitation, 15);
        const Trace b = simulateSteps(viaReal, LabExcitation(InputType::HARMONIC), 15);
        EXPECT_EQ(a.output, b.output);
    }

} // namespace
