// Проверка моделей ЛР №1: ручной расчёт, сходимость, граничные точки и инварианты.

#include <gtest/gtest.h>

#include <array>
#include <cmath>
#include <memory>
#include <ostream>
#include <string>

#include "InputSignal.h"
#include "Model.h"
#include "Model1_7.h"
#include "Model2_9.h"
#include "Model3_1.h"

namespace {

    constexpr double kTight = 1e-9;

    enum class Plant { Linear17, Sqrt29, Decay31 };

    std::unique_ptr<Model> makePlant(Plant plant) {
        switch (plant) {
        case Plant::Linear17:
            return std::make_unique<Model1_7>(0.6, 0.5, 0.3, 0.2);  // значения по умолчанию ЛР №1
        case Plant::Sqrt29:
            return std::make_unique<Model2_9>(0.5, 2.0);
        default:
            return std::make_unique<Model3_1>(1.0, 0.1, 1.0);
        }
    }

    // Подаём на модель воздействие из ЛР №1 и возвращаем выход после третьего такта
    double thirdOutput(Model& model, InputType kind) {
        double y = 0.0;
        for (int tau = 0; tau < 3; ++tau) {
            y = model.calculateNext(getInputValue(kind, tau));
        }
        return y;
    }

    struct HandRow {
        Plant plant;
        InputType kind;
        double y3;
    };

    std::ostream& operator<<(std::ostream& os, const HandRow& row) {
        static const std::array<const char*, 3> plants{ "M1_7", "M2_9", "M3_1" };
        static const std::array<const char*, 3> kinds{ "step", "impulse", "harmonic" };
        return os << plants[static_cast<int>(row.plant)] << '/'
                  << kinds[static_cast<int>(row.kind)] << " y3=" << row.y3;
    }

    // Расчёт вручную (см. readme, раздел 4.1)
    const HandRow kHandTable[] = {
        { Plant::Linear17, InputType::STEP,     1.66 },
        { Plant::Linear17, InputType::IMPULSE,  0.56 },
        { Plant::Linear17, InputType::HARMONIC, 0.9595313043 },
        { Plant::Sqrt29,   InputType::STEP,     3.5 },
        { Plant::Sqrt29,   InputType::IMPULSE,  0.5 },
        { Plant::Sqrt29,   InputType::HARMONIC, 2.8244590398 },
        { Plant::Decay31,  InputType::STEP,     0.729 },
        { Plant::Decay31,  InputType::IMPULSE,  0.729 },
        { Plant::Decay31,  InputType::HARMONIC, 0.729 },
    };

    class HandCalculation : public ::testing::TestWithParam<HandRow> {};

    TEST_P(HandCalculation, ThirdStepEqualsManualResult) {
        const HandRow& row = GetParam();
        auto model = makePlant(row.plant);
        EXPECT_NEAR(thirdOutput(*model, row.kind), row.y3, kTight);
    }

    INSTANTIATE_TEST_SUITE_P(Variant7, HandCalculation, ::testing::ValuesIn(kHandTable));

    // ---------------- Model 1.7: история управления ----------------

    TEST(InputHistory, ImpulseResponseRevealsEachCoefficient) {
        Model1_7 model(0.6, 0.5, 0.3, 0.2);
        EXPECT_DOUBLE_EQ(model.calculateNext(1.0), 0.5);       // b1
        EXPECT_NEAR(model.calculateNext(0.0), 0.6, kTight);    // a*b1 + b2
        EXPECT_NEAR(model.calculateNext(0.0), 0.56, kTight);   // a*(a*b1 + b2) + b3
        EXPECT_NEAR(model.calculateNext(0.0), 0.336, kTight);  // дальше только затухание с множителем a
    }

    TEST(InputHistory, OldInputsAreForgottenAfterTwoSteps) {
        Model1_7 model(0.0, 0.0, 0.0, 1.0);  // выход повторяет u(t-2)
        model.calculateNext(7.0);
        model.calculateNext(0.0);
        EXPECT_DOUBLE_EQ(model.calculateNext(0.0), 7.0);
        EXPECT_DOUBLE_EQ(model.calculateNext(0.0), 0.0);
    }

    // ---------------- Model 2.9: граничные точки корня со знаком ----------------

    class SignedRoot : public ::testing::TestWithParam<std::array<double, 2>> {};

    TEST_P(SignedRoot, FirstOutputFromZeroState) {
        const auto [u, expected] = GetParam();
        Model2_9 model(0.9, 3.0);
        EXPECT_NEAR(model.calculateNext(u), expected, kTight);
    }

    INSTANTIATE_TEST_SUITE_P(Points, SignedRoot, ::testing::Values(
        std::array<double, 2>{ 0.0, 0.0 },      // sign(0) = 0
        std::array<double, 2>{ 1.0, 3.0 },      // при |u| = 1 вход проходит без искажения
        std::array<double, 2>{ -1.0, -3.0 },
        std::array<double, 2>{ 0.04, 0.6 },     // |u| < 1 усиливается
        std::array<double, 2>{ -0.04, -0.6 },
        std::array<double, 2>{ 16.0, 12.0 },    // |u| > 1 ослабляется
        std::array<double, 2>{ -16.0, -12.0 }
    ));

    TEST(SignedRootSymmetry, NegativeInputMirrorsPositive) {
        for (double u : { 0.3, 2.5, 49.0 }) {
            Model2_9 plus(0.4, 1.5);
            Model2_9 minus(0.4, 1.5);
            EXPECT_DOUBLE_EQ(plus.calculateNext(u), -minus.calculateNext(-u));
        }
    }

    // ---------------- Model 3.1: схема Эйлера и точное решение ----------------

    TEST(EulerDecay, MatchesClosedFormOfDifferenceScheme) {
        const double a = 2.0;
        const double dt = 0.05;
        const double y0 = 3.0;
        Model3_1 model(a, dt, y0);
        for (int n = 1; n <= 40; ++n) {
            EXPECT_NEAR(model.calculateNext(0.0), y0 * std::pow(1.0 - a * dt, n), kTight) << "n=" << n;
        }
    }

    double eulerGap(double dt) {
        const double a = 1.5;
        Model3_1 model(a, dt, 1.0);
        const auto steps = static_cast<int>(std::lround(2.0 / dt));
        double y = 1.0;
        for (int n = 0; n < steps; ++n) {
            y = model.calculateNext(0.0);
        }
        return std::fabs(y - std::exp(-a * 2.0));
    }

    TEST(EulerDecay, ApproachesExponentialSolution) {
        EXPECT_LT(eulerGap(0.0005), 5e-4);
    }

    TEST(EulerDecay, HalvingStepHalvesError) {
        EXPECT_NEAR(eulerGap(0.002) / eulerGap(0.001), 2.0, 0.05);
    }

    TEST(EulerDecay, InputHasNoInfluence) {
        Model3_1 quiet(0.8, 0.2, 2.0);
        Model3_1 noisy(0.8, 0.2, 2.0);
        for (int tau = 0; tau < 10; ++tau) {
            EXPECT_DOUBLE_EQ(quiet.calculateNext(0.0), noisy.calculateNext(100.0 * tau - 300.0));
        }
    }

    TEST(EulerDecay, ResetReturnsToInitialCondition) {
        Model3_1 model(1.0, 0.1, 4.0);
        EXPECT_DOUBLE_EQ(model.getY(), 4.0);
        model.calculateNext(0.0);
        model.calculateNext(0.0);
        model.reset();
        EXPECT_DOUBLE_EQ(model.getY(), 4.0);
    }

    // ---------------- Установившиеся значения ----------------

    struct Settling {
        std::string name;
        std::shared_ptr<Model> model;
        double amplitude;
        double limit;
    };

    std::ostream& operator<<(std::ostream& os, const Settling& s) {
        return os << s.name;
    }

    class SettlingValue : public ::testing::TestWithParam<Settling> {};

    TEST_P(SettlingValue, LongStepRunReachesLimit) {
        const Settling& s = GetParam();
        s.model->reset();
        double y = 0.0;
        for (int tau = 0; tau < 4000; ++tau) {
            y = s.model->calculateNext(s.amplitude);
        }
        EXPECT_NEAR(y, s.limit, 1e-7);
    }

    INSTANTIATE_TEST_SUITE_P(Limits, SettlingValue, ::testing::Values(
        // Model 1.7: (b1 + b2 + b3) * A / (1 - a)
        Settling{ "m17_default", std::make_shared<Model1_7>(0.6, 0.5, 0.3, 0.2), 1.0, 2.5 },
        Settling{ "m17_negative_a", std::make_shared<Model1_7>(-0.5, 1.0, 1.0, 1.0), 2.0, 4.0 },
        // Model 2.9: b * sqrt(A) / (1 - a)
        Settling{ "m29_default", std::make_shared<Model2_9>(0.7, 1.0), 1.0, 1.0 / 0.3 },
        Settling{ "m29_amplitude9", std::make_shared<Model2_9>(0.25, 1.0), 9.0, 4.0 },
        // Model 3.1: свободное движение затухает до нуля при любом входе
        Settling{ "m31_default", std::make_shared<Model3_1>(1.0, 0.1, 1.0), 1.0, 0.0 },
        Settling{ "m31_oscillating", std::make_shared<Model3_1>(15.0, 0.1, 5.0), 1.0, 0.0 }
    ), [](const ::testing::TestParamInfo<Settling>& info) { return info.param.name; });

    // ---------------- Инварианты ----------------

    TEST(Invariants, RestStaysRest) {
        Model1_7 m17(0.6, 0.5, 0.3, 0.2);
        Model2_9 m29(0.7, 1.0);
        Model3_1 m31(1.0, 0.1, 0.0);  // нулевое начальное условие
        for (int tau = 0; tau < 12; ++tau) {
            EXPECT_EQ(m17.calculateNext(0.0), 0.0);
            EXPECT_EQ(m29.calculateNext(0.0), 0.0);
            EXPECT_EQ(m31.calculateNext(0.0), 0.0);
        }
    }

    TEST(Invariants, LinearModelObeysSuperposition) {
        Model1_7 first(0.3, 1.0, -0.5, 0.25);
        Model1_7 second(0.3, 1.0, -0.5, 0.25);
        Model1_7 both(0.3, 1.0, -0.5, 0.25);
        for (int tau = 0; tau < 9; ++tau) {
            const double p = std::cos(0.7 * tau);
            const double q = 0.1 * tau;
            const double sum = first.calculateNext(p) + second.calculateNext(q);
            EXPECT_NEAR(both.calculateNext(p + q), sum, kTight);
        }
    }

    TEST(Invariants, DecayScalesWithInitialCondition) {
        Model3_1 unit(0.7, 0.3, 1.0);
        Model3_1 scaled(0.7, 0.3, -2.5);
        for (int tau = 0; tau < 8; ++tau) {
            EXPECT_NEAR(scaled.calculateNext(0.0), -2.5 * unit.calculateNext(0.0), kTight);
        }
    }

    TEST(Invariants, ResetClearsHistory) {
        Model1_7 model(0.6, 0.5, 0.3, 0.2);
        const double first = thirdOutput(model, InputType::HARMONIC);
        model.reset();
        EXPECT_EQ(model.getY(), 0.0);
        EXPECT_DOUBLE_EQ(thirdOutput(model, InputType::HARMONIC), first);

        Model2_9 sqrtModel(0.5, 2.0);
        sqrtModel.calculateNext(4.0);
        sqrtModel.reset();
        EXPECT_EQ(sqrtModel.getY(), 0.0);
    }

} // namespace
