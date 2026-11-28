#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "models.h"
#include <cmath>
#include <stdexcept>
#include <vector>

using ::testing::Return;

// ---------- TEST_P для симуляции шагов Model 1.5 ----------
struct M15Param {
    double a1, a2, b;
    int k;
    std::vector<double> u;
    std::vector<double> expected;
};

class Model15ParamTest : public ::testing::TestWithParam<M15Param> {};

TEST_P(Model15ParamTest, MatchesHandComputed) {
    const auto& p = GetParam();
    Model15 m(p.a1, p.a2, p.b, p.k);
    m.reset();
    for (size_t i = 0; i < p.u.size(); ++i) {
        EXPECT_NEAR(m.step(p.u[i]), p.expected[i], 1e-9) << "step " << i;
    }
}

INSTANTIATE_TEST_SUITE_P(Default, Model15ParamTest, ::testing::Values(
    // Руками: 0.5, 0.8, 0.98, 1.188
    M15Param{0.6, 0.2, 0.5, 2, {1,1,1,1}, {0.5, 0.8, 0.98, 1.188}},
    // k=0: y = (a1+a2)*y_prev + b*u = 0.5*y + 1*u
    M15Param{0.5, 0.0, 1.0, 0, {1,1,1},    {1.0, 1.5, 1.75}},
    // Нулевой вход при нулевом старте -> всё нули (инвариант)
    M15Param{0.6, 0.2, 0.5, 2, {0,0,0},    {0.0, 0.0, 0.0}},
    // Импульс: y_next = 0.5*y_prev, u только на шаге 0
    M15Param{0.5, 0.0, 1.0, 0, {1,0,0,0},  {1.0, 0.5, 0.25, 0.125}}
));

TEST(Model15, ResetClearsState) {
    Model15 m(0.5, 0.0, 1.0, 0);
    m.reset();
    m.step(1.0); m.step(1.0);
    m.reset();
    EXPECT_DOUBLE_EQ(m.step(0.0), 0.0);
}

TEST(Model15, NegativeKThrows) {
    EXPECT_THROW(Model15(0.5, 0.0, 1.0, -1), std::invalid_argument);
}

// ---------- Model 2.7 ----------
TEST(Model27, ZeroInputAtZeroState) {
    Model27 m(0.7, 0.4, 0.1);
    m.reset();
    // По текущей логике: u=0, prev=0 -> r=-1 -> y=-b
    EXPECT_NEAR(m.step(0.0), -0.4, 1e-9);
}

TEST(Model27, PositiveInputDrives) {
    Model27 m(0.5, 1.0, 0.1);
    m.reset();
    EXPECT_NEAR(m.step(1.0), 1.0, 1e-9);   // r=1, y=0.5*0+1*1
    EXPECT_NEAR(m.step(1.0), 1.5, 1e-9);   // y=0.5*1+1*1
}

// Границы зоны нечувствительности (±eps)
TEST(Model27, HysteresisBoundaryPlusEps) {
    Model27 m(0.0, 1.0, 0.5);
    m.reset();
    // u=eps, prev=0 (<=0) -> r=-1
    EXPECT_NEAR(m.step(0.5), -1.0, 1e-9);
}

TEST(Model27, HysteresisBoundaryMinusEps) {
    Model27 m(0.0, 1.0, 0.5);
    m.reset();
    // Сначала залипаем в +1
    m.step(10.0);
    // u=-eps, prev=+1 (>0) -> r=+1
    EXPECT_NEAR(m.step(-0.5), 1.0, 1e-9);
}

TEST(Model27, NegativeEpsThrows) {
    EXPECT_THROW(Model27(0.5, 0.1, -0.1), std::invalid_argument);
}

// ---------- Model 3.9 ----------
TEST(Model39, IntegrationStep) {
    Model39 m(1.0, 0.1);
    m.reset();
    const double d = std::tanh(1.0) * 0.1;
    EXPECT_NEAR(m.step(1.0),     d, 1e-9);
    EXPECT_NEAR(m.step(1.0), 2.0 * d, 1e-9);
}

TEST(Model39, ZeroInputStays) {
    Model39 m(1.0, 0.1);
    m.reset();
    for (int i = 0; i < 10; ++i) m.step(0.0);
    EXPECT_DOUBLE_EQ(m.step(0.0), 0.0);
}

TEST(Model39, SaturationLargeInput) {
    Model39 m(1.0, 0.1);
    m.reset();
    // tanh(100) ~= 1 => y ~= 0.1
    EXPECT_NEAR(m.step(100.0), 0.1, 1e-6);
    // tanh(-100) ~= -1 => y ~= 0.1 + (-0.1) = 0
    EXPECT_NEAR(m.step(-100.0), 0.0, 1e-6);
}

// ---------- GMock: подмена входных сигналов ----------
class InputSource {
public:
    virtual ~InputSource() = default;
    virtual double at(int tau) const = 0;
};

class MockInputSource : public InputSource {
public:
    MOCK_METHOD(double, at, (int tau), (const, override));
};

TEST(MockInputTest, VerifyImpulseMode) {
    MockInputSource mock;
    EXPECT_CALL(mock, at(0)).WillOnce(Return(1.0));
    EXPECT_CALL(mock, at(1)).WillOnce(Return(0.0));
    EXPECT_CALL(mock, at(2)).WillOnce(Return(0.0));

    Model15 m(0.5, 0.0, 1.0, 0);
    m.reset();
    std::vector<double> out;
    for (int i = 0; i < 3; ++i) out.push_back(m.step(mock.at(i)));

    EXPECT_NEAR(out[0], 1.0,  1e-9);
    EXPECT_NEAR(out[1], 0.5,  1e-9);
    EXPECT_NEAR(out[2], 0.25, 1e-9);
}
