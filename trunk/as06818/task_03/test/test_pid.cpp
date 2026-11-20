#include <gtest/gtest.h>

#include "../src/pid.hpp"

// Нулевые коэффициенты: управление всегда нулевое.
TEST(PidTest, ZeroGivesZeroControl) {
    PidController pid(0.0, 0.0, 0.0, 1.0);
    EXPECT_DOUBLE_EQ(pid.compute(5.0), 0.0);
    EXPECT_DOUBLE_EQ(pid.compute(3.0), 0.0);
}

// Ручной расчёт инкрементной формы при Kp=0.5, Ki=0.1, Kd=0.2, dt=1:
//   e = 1.0:  du = 0.5*1 + 0.1*1 + 0.2*1 = 0.8      -> u = 0.8
//   e = 0.6:  du = -0.2 + 0.06 - 0.28 = -0.42       -> u = 0.38
//   e = -0.2: du = -0.4 - 0.02 - 0.08 = -0.5        -> u = -0.12
TEST(PidTest, IncrementalFormHandCalculated) {
    PidController pid(0.5, 0.1, 0.2, 1.0, -100.0, 100.0);
    EXPECT_NEAR(pid.compute(1.0), 0.8, 1e-12);
    EXPECT_NEAR(pid.compute(0.6), 0.38, 1e-12);
    EXPECT_NEAR(pid.compute(-0.2), -0.12, 1e-12);
}

// Чистый П-регулятор в инкрементной форме: u = Kp * e[t]
// (инкременты Kp*(e - e1) в сумме телескопируются).
TEST(PidTest, PureProportional) {
    PidController pid(2.0, 0.0, 0.0, 1.0);
    EXPECT_DOUBLE_EQ(pid.compute(1.0), 2.0);
    EXPECT_DOUBLE_EQ(pid.compute(1.0), 2.0);
    EXPECT_DOUBLE_EQ(pid.compute(0.5), 1.0);
    EXPECT_DOUBLE_EQ(pid.compute(-1.5), -3.0);
}

// Чистый И-регулятор: u = Ki * dt * sum(e).
TEST(PidTest, PureIntegralAccumulates) {
    PidController pid(0.0, 0.5, 0.0, 2.0);
    EXPECT_NEAR(pid.compute(0.2), 0.2, 1e-12);
    EXPECT_NEAR(pid.compute(0.2), 0.4, 1e-12);
    EXPECT_NEAR(pid.compute(0.2), 0.6, 1e-12);
    EXPECT_NEAR(pid.compute(-0.4), 0.2, 1e-12);
}

// Чистый Д-регулятор: du = Kd/dt * (e - 2*e1 + e2); Kd/dt = 3.
TEST(PidTest, PureDerivative) {
    PidController pid(0.0, 0.0, 1.5, 0.5);
    EXPECT_NEAR(pid.compute(1.0), 3.0, 1e-12);   // 3*(1 - 0 + 0) = 3
    EXPECT_NEAR(pid.compute(1.0), 0.0, 1e-12);   // 3*(1 - 2 + 0) = -3
    EXPECT_NEAR(pid.compute(1.0), 0.0, 1e-12);   // 3*(1 - 2 + 1) = 0
    EXPECT_NEAR(pid.compute(0.0), -3.0, 1e-12);  // 3*(0 - 2 + 1) = -3
}

// Постоянная ошибка при Ki = 0: после второго такта приращения нет
// (П-составляющая не меняется, производная равна нулю).
TEST(PidTest, ConstantErrorSettles) {
    PidController pid(1.0, 0.0, 0.5, 1.0);
    EXPECT_NEAR(pid.compute(2.0), 3.0, 1e-12);  // 1*2 + 0.5*2 = 3
    EXPECT_NEAR(pid.compute(2.0), 2.0, 1e-12);  // 0 + 0.5*(2 - 4) = -1
    EXPECT_NEAR(pid.compute(2.0), 2.0, 1e-12);
    EXPECT_NEAR(pid.compute(2.0), 2.0, 1e-12);
}

// Ограничение актуатора: u не выходит за [u_min, u_max] и,
// поскольку история хранит уже ограниченное значение, после
// насыщения управление не «залипает» выше предела (anti-windup).
TEST(PidTest, ActuatorLimitsAndAntiWindup) {
    PidController pid(10.0, 0.0, 0.0, 1.0, -5.0, 5.0);
    EXPECT_DOUBLE_EQ(pid.compute(100.0), 5.0);
    EXPECT_DOUBLE_EQ(pid.compute(100.0), 5.0);
    EXPECT_DOUBLE_EQ(pid.get_u(), 5.0);
    // Ошибка исчезла — инкремент сразу уводит u вниз, выброса нет.
    EXPECT_DOUBLE_EQ(pid.compute(0.0), -5.0);
    EXPECT_DOUBLE_EQ(pid.compute(0.0), -5.0);
}

// Интегральная составляющая при насыщении не накапливается: u стоит
// на пределе столько, сколько длится ошибка, и выходит из насыщения
// сразу при её уменьшении (а не после долгой «раскрутки» суммы).
TEST(PidTest, IntegralDoesNotWindUp) {
    PidController pid(0.0, 1.0, 0.0, 1.0, -2.0, 2.0);
    for (int i = 0; i < 10; ++i) {
        EXPECT_DOUBLE_EQ(pid.compute(50.0), 2.0);
    }
    // Ошибка обнулилась — инкремент нулевой, u сохраняется.
    EXPECT_DOUBLE_EQ(pid.compute(0.0), 2.0);
    // Без ограничения здесь было бы u = 500 - 1, а не 1.
    EXPECT_DOUBLE_EQ(pid.compute(-1.0), 1.0);
}

TEST(PidTest, ResetClearsHistory) {
    PidController pid(0.5, 0.1, 0.2, 1.0, -100.0, 100.0);
    pid.compute(1.0);
    pid.compute(0.6);
    pid.reset();
    EXPECT_DOUBLE_EQ(pid.get_u(), 0.0);
    // Первый такт после сброса повторяет расчёт с чистой историей.
    EXPECT_NEAR(pid.compute(1.0), 0.8, 1e-12);
}

TEST(PidTest, GainAccessors) {
    PidController pid(1.0, 0.5, 0.25, 1.0);
    EXPECT_DOUBLE_EQ(pid.get_kp(), 1.0);
    EXPECT_DOUBLE_EQ(pid.get_ki(), 0.5);
    EXPECT_DOUBLE_EQ(pid.get_kd(), 0.25);
}
