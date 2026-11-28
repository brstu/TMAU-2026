#include <gtest/gtest.h>

#include <cmath>
#include <string>

#include "Model.h"
#include "Model_1_1.h"
#include "Model_2_3.h"
#include "Model_3_5.h"
#include "Signal.h"

namespace
{
    template <class T>
    std::string caseName(const testing::TestParamInfo<T>& info)
    {
        return "case" + std::to_string(info.index);
    }

    //3 шага модели под действием выбраного сигнала (A = 1) и возвращает y(3)
    double runThreeSteps(const Model& model, SignalType type, double dt)
    {
        double y = 0.0;
        for (int tau = 0; tau < 3; ++tau)
            y = model.calculateNext(y, getInput(type, tau, 1.0), dt);
        return y;
    }

    //один набор данных: режим управления и значение y(3), посчитанное вручную.
    struct Case
    {
        SignalType type;
        double expected;
    };

    //Model 1.1: y(t+1) = a*y(t) + b*u(t), a = 0.8, b = 0.5

    class Model11Test : public testing::TestWithParam<Case> {};

    TEST_P(Model11Test, ThirdStepMatchesHandCalculation)
    {
        const Model1_1 model(0.8, 0.5);
        EXPECT_NEAR(runThreeSteps(model, GetParam().type, 1.0), GetParam().expected, 1e-8);
    }

    INSTANTIATE_TEST_SUITE_P(Modes, Model11Test, testing::Values(
        Case{SignalType::Step,     1.22},
        Case{SignalType::Impulse,  0.32},
        Case{SignalType::Harmonic, 0.791237107}
    ), caseName<Case>);

    //Model 2.3: y(t+1) = a*y(t) + b*deadzone(u(t)), a = 0.8, b = 0.5, delta = 0.3 

    class Model23Test : public testing::TestWithParam<Case> {};

    TEST_P(Model23Test, ThirdStepMatchesHandCalculation)
    {
        const Model2_3 model(0.8, 0.5, 0.3);
        EXPECT_NEAR(runThreeSteps(model, GetParam().type, 1.0), GetParam().expected, 1e-8);
    }

    INSTANTIATE_TEST_SUITE_P(Modes, Model23Test, testing::Values(
        Case{SignalType::Step,     0.854},
        Case{SignalType::Impulse,  0.224},
        Case{SignalType::Harmonic, 0.521237107}
    ), caseName<Case>);

    //Model 3.5: y(t+1) = y(t) + dt*b*sin(u(t)), b = 2, dt = 0.1

    class Model35Test : public testing::TestWithParam<Case> {};

    TEST_P(Model35Test, ThirdStepMatchesHandCalculation)
    {
        const Model3_5 model(2.0);
        EXPECT_NEAR(runThreeSteps(model, GetParam().type, 0.1), GetParam().expected, 1e-8);
    }

    INSTANTIATE_TEST_SUITE_P(Modes, Model35Test, testing::Values(
        Case{SignalType::Step,     0.504882591},
        Case{SignalType::Impulse,  0.168294197},
        Case{SignalType::Harmonic, 0.306939297}
    ), caseName<Case>);

    //граничные значения зоны нечувствительности (Model 2.3) 

    struct EdgeCase
    {
        double u;
        double expected;
    };

    class DeadzoneEdgeTest : public testing::TestWithParam<EdgeCase> {};

    TEST_P(DeadzoneEdgeTest, ValueAtAndAroundDelta)
    {
        const Model2_3 model(0.5, 2.0, 0.3);
        EXPECT_NEAR(model.calculateNext(0.0, GetParam().u, 1.0), GetParam().expected, 1e-12);
    }

    INSTANTIATE_TEST_SUITE_P(Edges, DeadzoneEdgeTest, testing::Values(
        EdgeCase{ 0.3, 0.0},
        EdgeCase{-0.3, 0.0},
        EdgeCase{ 0.0, 0.0},
        EdgeCase{ 0.5, 0.4},
        EdgeCase{-0.5, -0.4}
    ), caseName<EdgeCase>);

    //сходимость модели 1.1: y -> b*A/(1-a) при |a| < 1 

    struct SteadyCase
    {
        double a;
        double b;
        double amplitude;
    };

    class SteadyStateTest : public testing::TestWithParam<SteadyCase> {};

    TEST_P(SteadyStateTest, StepResponseConverges)
    {
        const SteadyCase c = GetParam();
        const Model1_1 model(c.a, c.b);

        double y = 0.0;
        for (int i = 0; i < 1000; ++i)
            y = model.calculateNext(y, c.amplitude, 1.0);

        EXPECT_NEAR(y, c.b * c.amplitude / (1.0 - c.a), 1e-6);
    }

    INSTANTIATE_TEST_SUITE_P(Convergence, SteadyStateTest, testing::Values(
        SteadyCase{0.8,  0.5, 1.0},
        SteadyCase{0.5,  1.0, 2.0},
        SteadyCase{-0.5, 3.0, 1.0}
    ), caseName<SteadyCase>);

    //Метод Эйлера и аналитическое решение (Model 3.5)

    //для u(t) = t уравнение dy/dt = sin(t) имеет точное решение y(1) = 1 - cos(1).
    double eulerError(double dt)
    {
        const int n = static_cast<int>(std::lround(1.0 / dt));
        const Model3_5 model(1.0);

        double y = 0.0;
        for (int k = 0; k < n; ++k)
            y = model.calculateNext(y, k * dt, dt);

        return std::fabs(y - (1.0 - std::cos(1.0)));
    }

    TEST(EulerTest, CloseToAnalyticSolution)
    {
        EXPECT_LT(eulerError(0.001), 1e-3);
    }

    TEST(EulerTest, SmallerStepGivesSmallerError)
    {
        EXPECT_LT(eulerError(0.005), eulerError(0.01));
    }

    //инварианты и названия

    TEST(InvariantTest, ZeroInputAndZeroStateGiveRest)
    {
        const Model1_1 m11(0.8, 0.5);
        const Model2_3 m23(0.8, 0.5, 0.3);
        const Model3_5 m35(2.0);

        EXPECT_DOUBLE_EQ(m11.calculateNext(0.0, 0.0, 1.0), 0.0);
        EXPECT_DOUBLE_EQ(m23.calculateNext(0.0, 0.0, 1.0), 0.0);
        EXPECT_DOUBLE_EQ(m35.calculateNext(0.0, 0.0, 0.1), 0.0);
    }

    TEST(NameTest, EachModelHasItsNumberInName)
    {
        const Model1_1 m11(0.8, 0.5);
        const Model2_3 m23(0.8, 0.5, 0.3);
        const Model3_5 m35(2.0);

        EXPECT_NE(std::string(m11.getName()).find("1.1"), std::string::npos);
        EXPECT_NE(std::string(m23.getName()).find("2.3"), std::string::npos);
        EXPECT_NE(std::string(m35.getName()).find("3.5"), std::string::npos);
    }
}
