#include <gtest/gtest.h>

#include <cmath>
#include <memory>
#include <ostream>
#include <string>

#include "InputSignal.h"
#include "Model.h"
#include "Model1_5.h"
#include "Model2_9.h"
#include "Model3_3.h"

namespace {

    template <class T>
    std::string caseName(const testing::TestParamInfo<T>& info) {
        return "case" + std::to_string(info.index);
    }

    enum class Mode { Step, Impulse, Harmonic };

    std::unique_ptr<InputSignal> makeInput(Mode mode) {
        switch (mode) {
        case Mode::Step:    return std::make_unique<StepSignal>();
        case Mode::Impulse: return std::make_unique<ImpulseSignal>();
        default:            return std::make_unique<HarmonicSignal>();
        }
    }

    // Три шага модели из нулевого состояния, возвращает y(3)
    double runSteps(Model& model, Mode mode, int steps = 3) {
        model.reset();
        const auto input = makeInput(mode);
        double y = 0.0;
        for (int t = 0; t < steps; ++t) {
            y = model.calculateNext(input->valueAt(t));
        }
        return y;
    }

    // Один набор данных: режим управления и y(3), посчитанное вручную
    struct Case {
        Mode mode;
        double expected;
    };

    std::ostream& operator<<(std::ostream& os, const Case& c) {
        const char* names[] = { "Step", "Impulse", "Harmonic" };
        return os << names[static_cast<int>(c.mode)] << ", y3=" << c.expected;
    }

    // ---------- Model 1.5: y(t+1) = a1*y(t) + a2*y(t-k) + b*u(t), a1 = 0.5, a2 = 0.3, b = 1, k = 1

    class Model15Test : public testing::TestWithParam<Case> {};

    TEST_P(Model15Test, ThirdStepMatchesHandCalculation) {
        Model1_5 model(0.5, 0.3, 1.0, 1);
        EXPECT_NEAR(runSteps(model, GetParam().mode), GetParam().expected, 1e-8);
    }

    INSTANTIATE_TEST_SUITE_P(Modes, Model15Test, testing::Values(
        Case{ Mode::Step,     2.05 },
        Case{ Mode::Impulse,  0.55 },
        Case{ Mode::Harmonic, 1.330032919 }
    ), caseName<Case>);

    // ---------- Model 2.9: y(t+1) = a*y(t) + b*sqrt(|u|)*sign(u), a = 0.7, b = 1

    class Model29Test : public testing::TestWithParam<Case> {};

    TEST_P(Model29Test, ThirdStepMatchesHandCalculation) {
        Model2_9 model(0.7, 1.0);
        EXPECT_NEAR(runSteps(model, GetParam().mode), GetParam().expected, 1e-8);
    }

    INSTANTIATE_TEST_SUITE_P(Modes, Model29Test, testing::Values(
        Case{ Mode::Step,     2.19 },
        Case{ Mode::Impulse,  0.49 },
        Case{ Mode::Harmonic, 1.595692975 }
    ), caseName<Case>);

    // ---------- Model 3.3: y(t+1) = y(t) + dt*(-a*y(t) + b*u(t)), a = 1, b = 1, dt = 0.1

    class Model33Test : public testing::TestWithParam<Case> {};

    TEST_P(Model33Test, ThirdStepMatchesHandCalculation) {
        Model3_3 model(1.0, 1.0, 0.1);
        EXPECT_NEAR(runSteps(model, GetParam().mode), GetParam().expected, 1e-8);
    }

    INSTANTIATE_TEST_SUITE_P(Modes, Model33Test, testing::Values(
        Case{ Mode::Step,     0.271 },
        Case{ Mode::Impulse,  0.081 },
        Case{ Mode::Harmonic, 0.166662131 }
    ), caseName<Case>);

    // ---------- Запаздывание Model 1.5: при k = 2 слагаемое a2*y(t-k) появляется только на 4-м шаге

    TEST(DelayTest, DelayedTermAppearsAfterKSteps) {
        Model1_5 model(0.5, 0.3, 1.0, 2);
        // значения совпадают с таблицей из лабораторной работы №1
        EXPECT_NEAR(model.calculateNext(1.0), 1.0, 1e-12);
        EXPECT_NEAR(model.calculateNext(1.0), 1.5, 1e-12);
        EXPECT_NEAR(model.calculateNext(1.0), 1.75, 1e-12);
        EXPECT_NEAR(model.calculateNext(1.0), 2.175, 1e-12);  // 0.5*1.75 + 0.3*1 + 1
    }

    TEST(DelayTest, NonPositiveDelayIsTreatedAsOne) {
        Model1_5 withZero(0.5, 0.3, 1.0, 0);
        Model1_5 withOne(0.5, 0.3, 1.0, 1);
        for (int t = 0; t < 5; ++t) {
            EXPECT_DOUBLE_EQ(withZero.calculateNext(1.0), withOne.calculateNext(1.0));
        }
    }

    // ---------- Граничные значения нелинейности Model 2.9 (корень со знаком)

    struct EdgeCase {
        double u;
        double expected;  // y(1) при y(0) = 0, a = 0.7, b = 2
    };

    std::ostream& operator<<(std::ostream& os, const EdgeCase& c) {
        return os << "u=" << c.u << ", y1=" << c.expected;
    }

    class SqrtNonlinearityTest : public testing::TestWithParam<EdgeCase> {};

    TEST_P(SqrtNonlinearityTest, FirstStepAtBoundaryPoints) {
        Model2_9 model(0.7, 2.0);
        EXPECT_NEAR(model.calculateNext(GetParam().u), GetParam().expected, 1e-12);
    }

    INSTANTIATE_TEST_SUITE_P(Edges, SqrtNonlinearityTest, testing::Values(
        EdgeCase{  0.0,   0.0 },   // точка перегиба: sign(0) = 0
        EdgeCase{  1.0,   2.0 },   // |u| = 1: нелинейность не меняет вход
        EdgeCase{ -1.0,  -2.0 },
        EdgeCase{  0.25,  1.0 },   // |u| < 1: вход усиливается, sqrt(0.25) = 0.5
        EdgeCase{ -0.25, -1.0 },
        EdgeCase{  4.0,   4.0 },   // |u| > 1: вход ослабляется, sqrt(4) = 2
        EdgeCase{ -4.0,  -4.0 }
    ), caseName<EdgeCase>);

    TEST(SqrtNonlinearityTest2, OddFunctionOfInput) {
        Model2_9 pos(0.7, 1.0);
        Model2_9 neg(0.7, 1.0);
        for (double u : { 0.1, 0.5, 2.0, 9.0 }) {
            EXPECT_DOUBLE_EQ(pos.calculateNext(u), -neg.calculateNext(-u));
        }
    }

    // ---------- Сходимость к установившемуся значению при ступенчатом входе A

    struct SteadyCase {
        double p1;  // a1 / a / a
        double p2;  // a2 / -  / dt
        double b;
        double amplitude;
    };

    std::ostream& operator<<(std::ostream& os, const SteadyCase& c) {
        return os << "p1=" << c.p1 << ", p2=" << c.p2 << ", b=" << c.b << ", A=" << c.amplitude;
    }

    double settle(Model& model, double amplitude) {
        double y = 0.0;
        for (int i = 0; i < 3000; ++i) {
            y = model.calculateNext(amplitude);
        }
        return y;
    }

    class Steady15Test : public testing::TestWithParam<SteadyCase> {};

    TEST_P(Steady15Test, ConvergesTo_bA_over_1_minus_a1_minus_a2) {
        const auto c = GetParam();
        Model1_5 model(c.p1, c.p2, c.b, 2);
        EXPECT_NEAR(settle(model, c.amplitude), c.b * c.amplitude / (1.0 - c.p1 - c.p2), 1e-6);
    }

    INSTANTIATE_TEST_SUITE_P(Convergence, Steady15Test, testing::Values(
        SteadyCase{ 0.5,  0.3, 1.0, 1.0 },   // y = 5 (значения по умолчанию)
        SteadyCase{ 0.2,  0.2, 2.0, 1.5 },
        SteadyCase{-0.4,  0.3, 1.0, 2.0 }
    ), caseName<SteadyCase>);

    class Steady29Test : public testing::TestWithParam<SteadyCase> {};

    TEST_P(Steady29Test, ConvergesTo_b_sqrtA_over_1_minus_a) {
        const auto c = GetParam();
        Model2_9 model(c.p1, c.b);
        EXPECT_NEAR(settle(model, c.amplitude), c.b * std::sqrt(c.amplitude) / (1.0 - c.p1), 1e-6);
    }

    INSTANTIATE_TEST_SUITE_P(Convergence, Steady29Test, testing::Values(
        SteadyCase{ 0.7, 0.0, 1.0, 1.0 },
        SteadyCase{ 0.5, 0.0, 2.0, 4.0 },
        SteadyCase{-0.5, 0.0, 1.5, 9.0 }
    ), caseName<SteadyCase>);

    class Steady33Test : public testing::TestWithParam<SteadyCase> {};

    TEST_P(Steady33Test, ConvergesTo_bA_over_a) {
        const auto c = GetParam();
        Model3_3 model(c.p1, c.b, c.p2);
        EXPECT_NEAR(settle(model, c.amplitude), c.b * c.amplitude / c.p1, 1e-6);
    }

    INSTANTIATE_TEST_SUITE_P(Convergence, Steady33Test, testing::Values(
        SteadyCase{ 1.0, 0.1,  1.0, 1.0 },
        SteadyCase{ 2.0, 0.05, 3.0, 1.0 },
        SteadyCase{ 0.5, 0.5,  1.0, 2.0 }
    ), caseName<SteadyCase>);

    // ---------- Метод Эйлера (Model 3.3) и аналитическое решение
    // dy/dt = -a*y + b, y(0) = 0  =>  y(t) = (b/a) * (1 - e^(-a*t))

    double eulerError(double dt) {
        const double a = 1.0;
        const double b = 2.0;
        const double T = 1.0;
        Model3_3 model(a, b, dt);
        const int n = static_cast<int>(std::lround(T / dt));
        double y = 0.0;
        for (int i = 0; i < n; ++i) {
            y = model.calculateNext(1.0);
        }
        return std::fabs(y - (b / a) * (1.0 - std::exp(-a * T)));
    }

    TEST(EulerTest, CloseToAnalyticSolution) {
        EXPECT_LT(eulerError(0.001), 1e-3);
    }

    TEST(EulerTest, ErrorIsFirstOrderInStep) {
        // у метода Эйлера ошибка ~ dt: при уменьшении шага вдвое ошибка падает примерно вдвое
        const double ratio = eulerError(0.01) / eulerError(0.005);
        EXPECT_NEAR(ratio, 2.0, 0.05);
    }

    TEST(EulerTest, OneStepIsExactEulerFormula) {
        Model3_3 model(2.0, 3.0, 0.2);
        // y1 = 0 + 0.2*(-2*0 + 3*1) = 0.6;  y2 = 0.6 + 0.2*(-1.2 + 3) = 0.96
        EXPECT_DOUBLE_EQ(model.calculateNext(1.0), 0.6);
        EXPECT_NEAR(model.calculateNext(1.0), 0.96, 1e-12);
    }

    // ---------- Инварианты и физический смысл

    TEST(InvariantTest, ZeroInputAndZeroStateGiveRest) {
        Model1_5 m15(0.5, 0.3, 1.0, 2);
        Model2_9 m29(0.7, 1.0);
        Model3_3 m33(1.0, 1.0, 0.1);
        for (int t = 0; t < 10; ++t) {
            EXPECT_DOUBLE_EQ(m15.calculateNext(0.0), 0.0);
            EXPECT_DOUBLE_EQ(m29.calculateNext(0.0), 0.0);
            EXPECT_DOUBLE_EQ(m33.calculateNext(0.0), 0.0);
        }
    }

    TEST(InvariantTest, ResetRestoresZeroState) {
        Model1_5 m15(0.5, 0.3, 1.0, 2);
        Model2_9 m29(0.7, 1.0);
        Model3_3 m33(1.0, 1.0, 0.1);

        const double first15 = runSteps(m15, Mode::Step);
        const double first29 = runSteps(m29, Mode::Step);
        const double first33 = runSteps(m33, Mode::Step);

        // runSteps вызывает reset(), поэтому повторный запуск даёт тот же результат
        EXPECT_DOUBLE_EQ(runSteps(m15, Mode::Step), first15);
        EXPECT_DOUBLE_EQ(runSteps(m29, Mode::Step), first29);
        EXPECT_DOUBLE_EQ(runSteps(m33, Mode::Step), first33);

        m15.reset();
        m29.reset();
        m33.reset();
        EXPECT_DOUBLE_EQ(m15.output(), 0.0);
        EXPECT_DOUBLE_EQ(m29.output(), 0.0);
        EXPECT_DOUBLE_EQ(m33.output(), 0.0);
    }

    TEST(InvariantTest, OutputReturnsLastCalculatedValue) {
        Model2_9 model(0.7, 1.0);
        const double y = model.calculateNext(4.0);
        EXPECT_DOUBLE_EQ(model.output(), y);
    }

    TEST(InvariantTest, LinearModelIsSuperposition) {
        // для линейных моделей реакция на сумму входов равна сумме реакций
        Model1_5 m1(0.5, 0.3, 1.0, 2);
        Model1_5 m2(0.5, 0.3, 1.0, 2);
        Model1_5 m12(0.5, 0.3, 1.0, 2);
        for (int t = 0; t < 8; ++t) {
            const double u1 = std::sin(t);
            const double u2 = t == 0 ? 1.0 : 0.0;
            EXPECT_NEAR(m12.calculateNext(u1 + u2), m1.calculateNext(u1) + m2.calculateNext(u2), 1e-12);
        }
    }

} // namespace
