// Модуль PoleAnalysis: вердикт по спектральному радиусу, полюса моделей,
// тревожные сообщения, исключение и согласие с поведением моделей во времени.

#include <gtest/gtest.h>

#include <cmath>
#include <limits>
#include <sstream>
#include <string>
#include <tuple>

#include "Model1_7.h"
#include "Model2_9.h"
#include "Model3_1.h"
#include "PoleAnalysis.h"

using stability::Verdict;

namespace {

    const double kNaN = std::numeric_limits<double>::quiet_NaN();
    const double kInf = std::numeric_limits<double>::infinity();

    // ---------------- Граница единичной окружности ----------------

    class UnitCircleEdge : public ::testing::TestWithParam<std::tuple<double, Verdict>> {};

    TEST_P(UnitCircleEdge, RadiusIsClassified) {
        const auto [radius, expected] = GetParam();
        EXPECT_EQ(stability::judge(radius), expected) << "radius " << radius;
    }

    INSTANTIATE_TEST_SUITE_P(Radii, UnitCircleEdge, ::testing::Values(
        std::make_tuple(0.0, Verdict::Converging),
        std::make_tuple(0.42, Verdict::Converging),
        std::make_tuple(0.9999, Verdict::Converging),
        std::make_tuple(1.0, Verdict::Marginal),
        std::make_tuple(1.0 + 1e-12, Verdict::Marginal),
        std::make_tuple(1.0 - 1e-12, Verdict::Marginal),
        std::make_tuple(1.0001, Verdict::Diverging),
        std::make_tuple(12.0, Verdict::Diverging),
        std::make_tuple(kInf, Verdict::Diverging),
        std::make_tuple(kNaN, Verdict::Diverging)
    ));

    TEST(SpectralRadius, LargestAbsolutePole) {
        EXPECT_DOUBLE_EQ(stability::spectralRadius({ 0.2, -0.9, 0.5 }), 0.9);
        EXPECT_DOUBLE_EQ(stability::spectralRadius({}), 0.0);
        EXPECT_TRUE(std::isinf(stability::spectralRadius({ 0.1, kNaN })));
    }

    // ---------------- Полюса моделей варианта 7 ----------------

    // Model 1.7: полюса 0, 0, a — на сходимость влияет только a, коэффициенты b нет
    class Linear17Poles : public ::testing::TestWithParam<std::tuple<double, Verdict>> {};

    TEST_P(Linear17Poles, OnlyCoefficientADecides) {
        const auto [a, expected] = GetParam();
        const auto report = stability::inspectModel1_7(a);
        ASSERT_EQ(report.poles.size(), 3u);
        EXPECT_EQ(report.poles[0], 0.0);
        EXPECT_EQ(report.poles[1], 0.0);
        EXPECT_EQ(report.poles[2], a);
        EXPECT_DOUBLE_EQ(report.spectralRadius, std::fabs(a));
        EXPECT_EQ(report.verdict, expected);
    }

    INSTANTIATE_TEST_SUITE_P(CoefficientA, Linear17Poles, ::testing::Values(
        std::make_tuple(0.6, Verdict::Converging),
        std::make_tuple(0.0, Verdict::Converging),
        std::make_tuple(-0.95, Verdict::Converging),
        std::make_tuple(1.0, Verdict::Marginal),
        std::make_tuple(-1.0, Verdict::Marginal),
        std::make_tuple(1.3, Verdict::Diverging),
        std::make_tuple(-2.0, Verdict::Diverging)
    ));

    class Sqrt29Poles : public ::testing::TestWithParam<std::tuple<double, Verdict>> {};

    TEST_P(Sqrt29Poles, PoleEqualsA) {
        const auto [a, expected] = GetParam();
        const auto report = stability::inspectModel2_9(a);
        ASSERT_EQ(report.poles.size(), 1u);
        EXPECT_EQ(report.poles.front(), a);
        EXPECT_EQ(report.verdict, expected);
    }

    INSTANTIATE_TEST_SUITE_P(CoefficientA, Sqrt29Poles, ::testing::Values(
        std::make_tuple(0.7, Verdict::Converging),
        std::make_tuple(-0.3, Verdict::Converging),
        std::make_tuple(1.0, Verdict::Marginal),
        std::make_tuple(-1.0, Verdict::Marginal),
        std::make_tuple(1.05, Verdict::Diverging),
        std::make_tuple(-3.0, Verdict::Diverging)
    ));

    // Model 3.1: полюс 1 - a*dt, схема сходится при 0 < a*dt < 2
    class Decay31Poles : public ::testing::TestWithParam<std::tuple<double, double, Verdict>> {};

    TEST_P(Decay31Poles, PoleDependsOnProductADt) {
        const auto [a, dt, expected] = GetParam();
        const auto report = stability::inspectModel3_1(a, dt);
        EXPECT_NEAR(report.poles.front(), 1.0 - a * dt, 1e-15);
        EXPECT_EQ(report.verdict, expected);
    }

    INSTANTIATE_TEST_SUITE_P(StepSize, Decay31Poles, ::testing::Values(
        std::make_tuple(1.0, 0.1, Verdict::Converging),    // полюс 0.9
        std::make_tuple(18.0, 0.1, Verdict::Converging),   // полюс -0.8: колебания затухают
        std::make_tuple(10.0, 0.2, Verdict::Marginal),     // a*dt = 2, полюс -1
        std::make_tuple(0.0, 0.5, Verdict::Marginal),      // a = 0, полюс 1
        std::make_tuple(30.0, 0.1, Verdict::Diverging),    // слишком крупный шаг
        std::make_tuple(-0.5, 0.1, Verdict::Diverging)     // сам объект неустойчив
    ));

    TEST(NonFinite, BrokenCoefficientsNeverConverge) {
        EXPECT_EQ(stability::inspectModel1_7(kNaN).verdict, Verdict::Diverging);
        EXPECT_EQ(stability::inspectModel2_9(kInf).verdict, Verdict::Diverging);
        EXPECT_EQ(stability::inspectModel3_1(1.0, kNaN).verdict, Verdict::Diverging);
    }

    TEST(ReportText, ContainsModelAndDenominator) {
        const auto report = stability::inspectModel1_7(0.25);
        EXPECT_EQ(report.modelTag, "Linear_1_7");
        EXPECT_NE(report.polynomial.find("0.25"), std::string::npos);
        EXPECT_EQ(stability::verdictText(Verdict::Converging), "converging");
        EXPECT_NE(stability::verdictText(Verdict::Marginal).find("unit circle"), std::string::npos);
        EXPECT_EQ(stability::verdictText(Verdict::Diverging), "DIVERGING");
    }

    // ---------------- Предупреждение и исключение ----------------

    TEST(Alarm, SilentForConvergingModel) {
        std::ostringstream sink;
        EXPECT_FALSE(stability::printAlarm(stability::inspectModel2_9(0.7), sink));
        EXPECT_EQ(sink.str(), "");
    }

    TEST(Alarm, PrintedForDivergingModel) {
        std::ostringstream sink;
        EXPECT_TRUE(stability::printAlarm(stability::inspectModel1_7(1.3), sink));
        EXPECT_NE(sink.str().find("Linear_1_7"), std::string::npos);
        EXPECT_NE(sink.str().find("DIVERGING"), std::string::npos);
    }

    TEST(Alarm, PrintedForMarginalModel) {
        std::ostringstream sink;
        EXPECT_TRUE(stability::printAlarm(stability::inspectModel3_1(10.0, 0.2), sink));
        EXPECT_NE(sink.str().find("marginal"), std::string::npos);
    }

    TEST(Enforce, ThrowsOnlyWhenNotConverging) {
        EXPECT_NO_THROW(stability::enforceConvergence(stability::inspectModel3_1(1.0, 0.1)));
        EXPECT_THROW(stability::enforceConvergence(stability::inspectModel3_1(30.0, 0.1)),
                     stability::DivergenceError);
        EXPECT_THROW(stability::enforceConvergence(stability::inspectModel2_9(1.0)),
                     stability::DivergenceError);
    }

    // ---------------- Согласие с ЛР №1 и с поведением моделей ----------------

    TEST(Lab1Agreement, IsStableMethodsGiveSameAnswer) {
        std::string note;
        for (double a : { 0.6, -0.4, 1.3, -1.7 }) {
            EXPECT_EQ(Model1_7(a, 0.5, 0.3, 0.2).isStable(note),
                      stability::converges(stability::inspectModel1_7(a)));
            EXPECT_EQ(Model2_9(a, 1.0).isStable(note),
                      stability::converges(stability::inspectModel2_9(a)));
        }
        for (double dt : { 0.1, 1.5, 2.5 }) {
            EXPECT_EQ(Model3_1(1.0, dt, 1.0).isStable(note),
                      stability::converges(stability::inspectModel3_1(1.0, dt)));
        }
        EXPECT_FALSE(note.empty());
    }

    // Свободное движение после импульса: при сходимости затухает, иначе растёт
    double tailAfterImpulse(Model& model, int steps) {
        double y = model.calculateNext(1.0);
        for (int tau = 1; tau < steps; ++tau) {
            y = model.calculateNext(0.0);
        }
        return std::fabs(y);
    }

    TEST(Behaviour, ConvergingModelsForgetImpulse) {
        Model1_7 m17(0.6, 0.5, 0.3, 0.2);
        Model2_9 m29(-0.7, 1.0);
        Model3_1 m31(1.0, 0.1, 1.0);
        EXPECT_LT(tailAfterImpulse(m17, 200), 1e-8);
        EXPECT_LT(tailAfterImpulse(m29, 200), 1e-8);
        EXPECT_LT(tailAfterImpulse(m31, 200), 1e-8);
    }

    TEST(Behaviour, DivergingModelsBlowUp) {
        Model1_7 m17(1.3, 0.5, 0.3, 0.2);
        Model2_9 m29(-1.5, 1.0);
        Model3_1 m31(30.0, 0.1, 1.0);
        EXPECT_GT(tailAfterImpulse(m17, 80), 1e6);
        EXPECT_GT(tailAfterImpulse(m29, 80), 1e6);
        EXPECT_GT(tailAfterImpulse(m31, 80), 1e6);
    }

    // Управляющие символы заменяются, обычный текст отчёта не меняется
    TEST(Sanitize, ReplacesControlCharacters) {
        EXPECT_EQ(stability::sanitizeForLog("a\nb\rc\td"), "a_b_c_d");
        EXPECT_EQ(stability::sanitizeForLog("z^2 * (z - 0.6)"), "z^2 * (z - 0.6)");
        EXPECT_EQ(stability::sanitizeForLog(""), "");
    }

} // namespace
