#include <gtest/gtest.h>
#include <cmath>
#include <string>
#include "model.h"
#include "model1-8.h"
#include "model2-10.h"
#include "model3-2.h"
#include "input_signal.h"

namespace
{
    template <class T>
    std::string labelOf(const testing::TestParamInfo<T>& info)
    {
        return "case" + std::to_string(info.index);
    }

    // 3 шага, возвращает y(3)
    double simulate3(Model& m, InputType sig, double amp)
    {
        m.setInitial(0.0);
        double u_prev1 = 0.0, u_prev2 = 0.0, y = 0.0;
        for (int tau = 1; tau <= 3; ++tau)
        {
            const double u = InputSignal::getSignal(sig, tau, amp);
            y = m.calculateStep(u, u_prev1, u_prev2);
            u_prev2 = u_prev1;
            u_prev1 = u;
        }
        return y;
    }

    struct Case
    {
        InputType type;
        double expected;
    };

    // ArModel(0.8, 0.2, 0.5, 0.3), y0 = 0, A = 1
    class ArModelTest : public testing::TestWithParam<Case> {};
    TEST_P(ArModelTest, ThirdStepMatchesHandCalculation)
    {
        ArModel m(0.8, 0.2, 0.5, 0.3);
        EXPECT_NEAR(simulate3(m, GetParam().type, 1.0), GetParam().expected, 1e-8);
    }
    INSTANTIATE_TEST_SUITE_P(Modes, ArModelTest, testing::Values(
        Case{InputType::Step,     1.86},
        Case{InputType::Impulse,  0.66},
        Case{InputType::Harmonic, 1.2624390527811225}
    ), labelOf<Case>);

    // TanhModel(alpha = 0.8, beta = 0.5), y0 = 0, A = 1
    class TanhModelTest : public testing::TestWithParam<Case> {};
    TEST_P(TanhModelTest, ThirdStepMatchesHandCalculation)
    {
        TanhModel m(0.8, 0.5);
        EXPECT_NEAR(simulate3(m, GetParam().type, 1.0), GetParam().expected, 1e-8);
    }
    INSTANTIATE_TEST_SUITE_P(Modes, TanhModelTest, testing::Values(
        Case{InputType::Step,     1.0609747899636393},
        Case{InputType::Impulse,  0.28297903080407183},
        Case{InputType::Harmonic, 0.43525967154369111}
    ), labelOf<Case>);

    // IntegratorModel(beta = 2.0), y0 = 0, A = 1
    class IntegratorModelTest : public testing::TestWithParam<Case> {};
    TEST_P(IntegratorModelTest, ThirdStepMatchesHandCalculation)
    {
        IntegratorModel m(2.0);
        EXPECT_NEAR(simulate3(m, GetParam().type, 1.0), GetParam().expected, 1e-8);
    }
    INSTANTIATE_TEST_SUITE_P(Modes, IntegratorModelTest, testing::Values(
        Case{InputType::Step,     6.0},
        Case{InputType::Impulse,  2.0},
        Case{InputType::Harmonic, 3.7837768393868907}
    ), labelOf<Case>);

    // Граничные значения для Tanh: u^3 симметричен
    struct EdgeCase
    {
        double u;
        double expected;
    };
    class EdgeTest : public testing::TestWithParam<EdgeCase> {};
    TEST_P(EdgeTest, ValueAtAndAroundZero)
    {
        TanhModel m(0.5, 2.0);
        m.setInitial(0.0);
        EXPECT_NEAR(m.calculateStep(GetParam().u, 0.0, 0.0),
                    GetParam().expected, 1e-12);
    }
    INSTANTIATE_TEST_SUITE_P(Edges, EdgeTest, testing::Values(
        EdgeCase{ 0.0,  0.0},
        EdgeCase{ 0.3,  0.054},
        EdgeCase{-0.3, -0.054},
        EdgeCase{ 0.5,  0.25},
        EdgeCase{-0.5, -0.25}
    ), labelOf<EdgeCase>);

    // Сходимость ArModel при ступеньке: y_уст = (b1 + b2) * A / (1 - a1 - a2)
    struct SteadyCase
    {
        double a1, a2, b1, b2, amp;
    };
    class SteadyTest : public testing::TestWithParam<SteadyCase> {};
    TEST_P(SteadyTest, StepResponseConverges)
    {
        const SteadyCase c = GetParam();
        ArModel m(c.a1, c.a2, c.b1, c.b2);
        m.setInitial(0.0);
        double u_prev1 = 0.0, u_prev2 = 0.0, y = 0.0;
        for (int i = 0; i < 1000; ++i)
        {
            const double u = c.amp;
            y = m.calculateStep(u, u_prev1, u_prev2);
            u_prev2 = u_prev1;
            u_prev1 = u;
        }
        const double expected = (c.b1 + c.b2) * c.amp / (1.0 - c.a1 - c.a2);
        EXPECT_NEAR(y, expected, 1e-4);
    }
    INSTANTIATE_TEST_SUITE_P(Convergence, SteadyTest, testing::Values(
        SteadyCase{0.8, 0.1, 0.5, 0.3, 1.0},
        SteadyCase{0.5, 0.2, 1.0, 0.5, 2.0}
    ), labelOf<SteadyCase>);

    // Интегратор: y(n) = beta * sum_{k=1..n} u(k)
    TEST(IntegratorTest, CumulativeSum)
    {
        IntegratorModel m(2.0);
        m.setInitial(0.0);
        double y = 0.0, u_prev1 = 0.0, u_prev2 = 0.0;
        double expected = 0.0;
        for (int k = 1; k <= 5; ++k)
        {
            const double u = static_cast<double>(k);
            y = m.calculateStep(u, u_prev1, u_prev2);
            u_prev2 = u_prev1; u_prev1 = u;
            expected += 2.0 * u;
        }
        EXPECT_NEAR(y, expected, 1e-9);
    }

    TEST(InvariantTest, ZeroInputAndZeroStateGiveRest)
    {
        ArModel ar(0.8, 0.2, 0.5, 0.3);
        TanhModel th(0.8, 0.5);
        IntegratorModel in(2.0);
        ar.setInitial(0.0); th.setInitial(0.0); in.setInitial(0.0);
        EXPECT_DOUBLE_EQ(ar.calculateStep(0, 0, 0), 0.0);
        EXPECT_DOUBLE_EQ(th.calculateStep(0, 0, 0), 0.0);
        EXPECT_DOUBLE_EQ(in.calculateStep(0, 0, 0), 0.0);
    }

    TEST(NameTest, EachModelHasItsName)
    {
        ArModel ar(0.8, 0.2, 0.5, 0.3);
        TanhModel th(0.8, 0.5);
        IntegratorModel in(2.0);
        EXPECT_NE(ar.getName().find("AR"), std::string::npos);
        EXPECT_NE(th.getName().find("Tanh"), std::string::npos);
        EXPECT_NE(in.getName().find("Integrator"), std::string::npos);
    }
}