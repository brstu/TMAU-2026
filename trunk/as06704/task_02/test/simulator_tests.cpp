#include "input_signal.h"
#include "model.h"
#include "simulator.h"

#include <gtest/gtest.h>

#include <cmath>
#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace {

enum class ModelKind { Linear12, Nonlinear26, Differential310 };
enum class SignalKind { Step, Impulse, Harmonic };

struct SimulationCase {
    std::string name;
    ModelKind modelKind;
    SignalKind signalKind;
    std::size_t steps;
    double initialY;
    double dt;
    std::vector<double> expectedOutput;
};

std::unique_ptr<Model> makeModel(ModelKind kind) {
    switch (kind) {
    case ModelKind::Linear12:
        return std::make_unique<LinearModel12>(0.5, 1.0, 1);
    case ModelKind::Nonlinear26:
        return std::make_unique<NonlinearModel26>(0.1, 0.2, 0.5);
    case ModelKind::Differential310:
        return std::make_unique<DifferentialModel310>(0.5, 0.2);
    }
    return nullptr;
}

std::unique_ptr<InputSignal> makeSignal(SignalKind kind) {
    switch (kind) {
    case SignalKind::Step:
        return std::make_unique<StepSignal>();
    case SignalKind::Impulse:
        return std::make_unique<ImpulseSignal>();
    case SignalKind::Harmonic:
        return std::make_unique<HarmonicSignal>();
    }
    return nullptr;
}

class SimulatorParameterizedTest : public testing::TestWithParam<SimulationCase> {};

class FunctionalTestSignal final : public InputSignal {
public:
    explicit FunctionalTestSignal(std::function<double(std::size_t)> function)
        : function_(std::move(function)) {}

    double value(std::size_t tau) const override {
        return function_(tau);
    }

    std::string name() const override {
        return "Подмененный тестовый сигнал";
    }

private:
    std::function<double(std::size_t)> function_;
};

TEST_P(SimulatorParameterizedTest, CalculatesExpectedOutputSequence) {
    const SimulationCase& testCase = GetParam();
    const auto model = makeModel(testCase.modelKind);
    const auto signal = makeSignal(testCase.signalKind);

    ASSERT_NE(model, nullptr);
    ASSERT_NE(signal, nullptr);

    const Simulator simulator;
    const std::vector<Sample> samples = simulator.run(
        *model, *signal, testCase.steps, testCase.initialY, testCase.dt
    );

    ASSERT_EQ(samples.size(), testCase.expectedOutput.size());
    for (std::size_t i = 0; i < samples.size(); ++i) {
        EXPECT_NEAR(samples[i].output, testCase.expectedOutput[i], 1e-10)
            << "Ошибка на шаге " << i << " в сценарии " << testCase.name;
        EXPECT_DOUBLE_EQ(samples[i].input, signal->value(i));
    }
}

INSTANTIATE_TEST_SUITE_P(
    Variant12ModelsAndSignals,
    SimulatorParameterizedTest,
    testing::Values(
        SimulationCase{"Linear12_Step", ModelKind::Linear12, SignalKind::Step,
                       3, 0.0, 1.0, {0.0, 0.0, 1.0, 1.5}},
        SimulationCase{"Linear12_Impulse", ModelKind::Linear12, SignalKind::Impulse,
                       3, 0.0, 1.0, {0.0, 0.0, 1.0, 0.5}},
        SimulationCase{"Linear12_Harmonic", ModelKind::Linear12, SignalKind::Harmonic,
                       3, 0.0, 1.0, {0.0, 0.0, 0.0, std::sin(1.0)}},
        SimulationCase{"Nonlinear26_Impulse", ModelKind::Nonlinear26, SignalKind::Impulse,
                       3, 0.0, 1.0, {0.0, 0.5, 0.0125, -0.0999998046875}},
        SimulationCase{"Differential310_Step", ModelKind::Differential310, SignalKind::Step,
                       3, 0.0, 0.1, {0.0, 0.12, 0.234, 0.3423}},
        SimulationCase{"Differential310_Impulse", ModelKind::Differential310, SignalKind::Impulse,
                       3, 0.0, 0.1, {0.0, 0.12, 0.134, 0.1473}},
        SimulationCase{"Differential310_Harmonic", ModelKind::Differential310, SignalKind::Harmonic,
                       2, 0.0, 0.1, {0.0, 0.02, 0.12314709848078965}}
    ),
    [](const testing::TestParamInfo<SimulationCase>& info) {
        return info.param.name;
    }
);

TEST(SimulatorTest, ZeroStepsReturnsInitialSample) {
    const LinearModel12 model(0.5, 1.0, 1);
    const StepSignal signal;
    const Simulator simulator;

    const auto samples = simulator.run(model, signal, 0, 3.5, 1.0);

    ASSERT_EQ(samples.size(), 1U);
    EXPECT_EQ(samples[0].tau, 0U);
    EXPECT_DOUBLE_EQ(samples[0].input, 1.0);
    EXPECT_DOUBLE_EQ(samples[0].output, 3.5);
}

TEST(SimulatorTest, StopsWhenModelProducesNonFiniteOutput) {
    const NonlinearModel26 model(1e308, 0.2, 0.5);
    const StepSignal signal;
    const Simulator simulator;

    const auto samples = simulator.run(model, signal, 3, 1e200, 1.0);

    ASSERT_EQ(samples.size(), 1U);
    EXPECT_DOUBLE_EQ(samples.front().output, 1e200);
}

TEST(SimulatorTest, AcceptsSubstitutedInputSignal) {
    const DifferentialModel310 model(0.0, 0.0);
    const FunctionalTestSignal signal([](std::size_t tau) {
        return tau == 0 ? 1.0 : 0.0;
    });
    const Simulator simulator;

    const auto samples = simulator.run(model, signal, 2, 0.0, 0.1);

    ASSERT_EQ(samples.size(), 3U);
    EXPECT_NEAR(samples[1].output, 0.1, 1e-12);
    EXPECT_NEAR(samples[2].output, 0.1, 1e-12);
}

} // namespace
