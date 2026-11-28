#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <cmath>
#include <memory>
#include <tuple>

#include "LinearModel19.h"
#include "NonlinearModel23.h"
#include "ODE37.h"
#include "SignalGenerator.h"
#include "Simulator.h"

using ::testing::Return;

class MockSignal : public ISignalGenerator {
public:
    MOCK_METHOD(double, get, (double tau), (const, override));
};

// --- Google Mock: подмена входного сигнала ---------------------------------
TEST(SimulatorTest, UsesMockSignal) {
    auto model = std::make_unique<LinearModel19>(0.5, 0.1, 0.0, 1.0);
    Simulator simulator(std::move(model), 3);
    MockSignal signal;

    EXPECT_CALL(signal, get(0.0)).WillOnce(Return(1.0));
    EXPECT_CALL(signal, get(1.0)).WillOnce(Return(0.0));
    EXPECT_CALL(signal, get(2.0)).WillOnce(Return(0.0));

    const auto& y = simulator.run(signal);
    ASSERT_EQ(y.size(), 3u);
    EXPECT_DOUBLE_EQ(y[0], 1.0);   // y1 = 1*1
    EXPECT_DOUBLE_EQ(y[1], 0.5);   // y2 = 0.5*1
    EXPECT_DOUBLE_EQ(y[2], 0.35);  // y3 = 0.5*0.5 + 0.1*1
}

// --- Параметризованные тесты модели 1.9 (TEST_P) ----------------------------
class LinearModel19Test
    : public ::testing::TestWithParam<std::tuple<double, double, double, double,
    double, double, double,
    double, double, double>> {};

TEST_P(LinearModel19Test, StepCalculation) {
    const auto& [a1, a2, a3, b1, u1, u2, u3, e1, e2, e3] = GetParam(); // NOSONAR
    LinearModel19 model(a1, a2, a3, b1);

    EXPECT_NEAR(model.step(u1), e1, 1e-9);
    EXPECT_NEAR(model.step(u2), e2, 1e-9);
    EXPECT_NEAR(model.step(u3), e3, 1e-9);
}

INSTANTIATE_TEST_SUITE_P(
    Variant19,
    LinearModel19Test,
    ::testing::Values(
        // a1, a2, a3, b1, u1, u2, u3, y1, y2, y3
        std::make_tuple(0.4, 0.3, 0.1, 1.0, 1.0, 0.0, 0.0, 1.0, 0.40, 0.460),
        std::make_tuple(0.5, -0.2, 0.1, 2.0, 1.0, 1.0, 1.0, 2.0, 3.00, 3.100),
        std::make_tuple(0.0, 0.0, 0.0, 1.0, -1.0, 2.0, 0.0, -1.0, 2.00, 0.000),
        std::make_tuple(1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.00, 1.000)
    )
);

TEST(LinearModel19Behavior, ThirdOrderMemory) {
    LinearModel19 model(0.4, 0.3, 0.2, 1.0);
    EXPECT_NEAR(model.step(1.0), 1.00, 1e-9);
    EXPECT_NEAR(model.step(0.0), 0.40, 1e-9);
    EXPECT_NEAR(model.step(0.0), 0.46, 1e-9);
    EXPECT_NEAR(model.step(0.0), 0.504, 1e-9); // 0.4*0.46 + 0.3*0.4 + 0.2*1
}

TEST(LinearModel19Behavior, InitialStateAndReset) {
    LinearModel19 model(0.4, 0.3, 0.1, 1.0);
    model.setInitialState({ 1.0, 2.0, 3.0 });
    EXPECT_NEAR(model.step(0.0), 1.3, 1e-9); // 0.4*1 + 0.3*2 + 0.1*3

    model.reset();
    EXPECT_DOUBLE_EQ(model.step(0.0), 0.0);
}

TEST(LinearModel19Behavior, Name) {
    LinearModel19 model(0.4, 0.3, 0.1, 1.0);
    EXPECT_STREQ(model.name(), "Model_1_9");
}

// --- Параметризованные тесты нелинейной модели 2.3 --------------------------
class NonlinearModel23Test
    : public ::testing::TestWithParam<std::tuple<double, double, double,
    double, double, double,
    double, double, double>> {};

TEST_P(NonlinearModel23Test, StepCalculation) {
    const auto& [a1, b1, c1, u1, u2, u3, e1, e2, e3] = GetParam(); // NOSONAR
    NonlinearModel23 model(a1, b1, c1);

    EXPECT_NEAR(model.step(u1), e1, 1e-9);
    EXPECT_NEAR(model.step(u2), e2, 1e-9);
    EXPECT_NEAR(model.step(u3), e3, 1e-9);
}

INSTANTIATE_TEST_SUITE_P(
    Variant19,
    NonlinearModel23Test,
    ::testing::Values(
        // a1, b1, c1, u1, u2, u3, y1, y2, y3
        std::make_tuple(0.5, 0.1, 2.0, 1.0, 0.0, 0.0, 2.0, 1.00, 0.100),
        std::make_tuple(0.0, 0.0, 1.0, -1.0, 0.0, 0.0, -1.0, 0.00, 0.000),
        std::make_tuple(0.8, 0.2, 1.0, 2.0, 0.0, 0.0, 2.0, 1.60, 0.480)
    )
);

TEST(NonlinearModel23Behavior, NameAndReset) {
    NonlinearModel23 model(0.5, 0.1, 2.0);
    EXPECT_STREQ(model.name(), "Model_2_3");

    // Граница sign(u) = 0 и базовая реализация setInitialState
    model.setInitialState({});
    model.reset();
    EXPECT_DOUBLE_EQ(model.step(0.0), 0.0);
}

// --- Параметризованные тесты дифференциального уравнения 3.7 -----------------
class ODE37Test
    : public ::testing::TestWithParam<std::tuple<double, double, double,
    double, double, double>> {};

TEST_P(ODE37Test, EulerStep) {
    const auto& [a, b, dt, u, e1, e2] = GetParam(); // NOSONAR
    ODE37 model(a, b, dt);

    EXPECT_NEAR(model.step(u), e1, 1e-9);
    EXPECT_NEAR(model.step(u), e2, 1e-9);
}

INSTANTIATE_TEST_SUITE_P(
    Variant19,
    ODE37Test,
    ::testing::Values(
        // a, b, dt, u, y1, y2
        std::make_tuple(0.0, 1.0, 0.1, 1.0, 0.100, 0.190),
        std::make_tuple(std::log(2.0), 1.0, 0.1, 1.0, 0.100, 0.180),
        std::make_tuple(0.0, 0.0, 0.5, 3.0, 0.000, 0.000),
        std::make_tuple(0.0, 2.0, 0.25, 1.0, 0.500, 0.875)
    )
);

TEST(ODE37Behavior, NameAndReset) {
    ODE37 model(0.0, 1.0, 0.1);
    EXPECT_STREQ(model.name(), "Model_3_7");
    model.step(1.0);
    model.reset();
    EXPECT_DOUBLE_EQ(model.step(0.0), 0.0);
}

// --- Генератор сигналов ------------------------------------------------------
class SignalGeneratorTest
    : public ::testing::TestWithParam<std::tuple<InputType, double, double, double>> {};

TEST_P(SignalGeneratorTest, ValueAtTime) {
    const auto& [type, amplitude, tau, expected] = GetParam(); // NOSONAR
    SignalGenerator signal(type, amplitude);
    EXPECT_NEAR(signal.get(tau), expected, 1e-9);
}

INSTANTIATE_TEST_SUITE_P(
    AllInputModes,
    SignalGeneratorTest,
    ::testing::Values(
        std::make_tuple(InputType::STEP, 2.0, 5.0, 2.0),
        std::make_tuple(InputType::IMPULSE, 2.0, 0.0, 2.0),
        std::make_tuple(InputType::IMPULSE, 2.0, 1.0, 0.0),
        std::make_tuple(InputType::HARMONIC, 3.0, std::acos(-1.0) / 2.0, 3.0)
    )
);

TEST(SignalGeneratorTest, InvalidTypeFallback) {
    SignalGenerator invalid(static_cast<InputType>(99), 1.0);
    EXPECT_DOUBLE_EQ(invalid.get(0.0), 0.0);
}

TEST(SimulatorTest, RealGeneratorValues) {
    auto model = std::make_unique<LinearModel19>(0.4, 0.3, 0.1, 1.0);
    Simulator simulator(std::move(model), 3);
    SignalGenerator signal(InputType::STEP, 1.0);

    const auto& y = simulator.run(signal);
    ASSERT_EQ(y.size(), 3u);
    EXPECT_NEAR(y[0], 1.00, 1e-9);
    EXPECT_NEAR(y[1], 1.40, 1e-9); // ступенчатый сигнал подаётся на каждом шаге
    EXPECT_NEAR(y[2], 1.86, 1e-9);
}

TEST(InvariantTest, ZeroInputZeroState) {
    LinearModel19 linear(0.4, 0.3, 0.1, 1.0);
    NonlinearModel23 nonlinear(0.5, 0.1, 2.0);
    ODE37 ode(0.0, 1.0, 0.1);

    EXPECT_DOUBLE_EQ(linear.step(0.0), 0.0);
    EXPECT_DOUBLE_EQ(nonlinear.step(0.0), 0.0);
    EXPECT_DOUBLE_EQ(ode.step(0.0), 0.0);
}