#include <gtest/gtest.h>

#include "LinearModel11.h"
#include "LinearModel13.h"
#include "NonlinearModel22.h"
#include "NonlinearModel23.h"
#include "NonlinearModel25.h"
#include "ODEModel37.h"
#include "StabilityAnalyzer.h"

#include <cmath>
#include <sstream>
#include <vector>

namespace {

// Модель первого порядка: |a| < 1 — процесс устойчив.
TEST(StabilityAnalyzerTest, FirstOrderModelIsStableInsideUnitCircle) {
    const auto result = StabilityAnalyzer::analyzeFirstOrder(0.5);

    EXPECT_TRUE(result.isStable());
    EXPECT_FALSE(result.isUnstable());
    EXPECT_FALSE(result.isMarginal());
    ASSERT_EQ(1u, result.roots.size());
    EXPECT_NEAR(0.5, result.moduli[0], 1e-12);
}

// Модель первого порядка: |a| > 1 — процесс расходится.
TEST(StabilityAnalyzerTest, FirstOrderModelIsUnstableOutsideUnitCircle) {
    const auto result = StabilityAnalyzer::analyzeFirstOrder(1.5);

    EXPECT_FALSE(result.isStable());
    EXPECT_TRUE(result.isUnstable());
    EXPECT_GT(result.maxModulus(), 1.0);
}

// Граничные значения |z| = 1 — процесс на границе устойчивости.
TEST(StabilityAnalyzerTest, FirstOrderModelOnUnitCircleIsMarginal) {
    const auto atPlusOne = StabilityAnalyzer::analyzeFirstOrder(1.0);
    const auto atMinusOne = StabilityAnalyzer::analyzeFirstOrder(-1.0);

    EXPECT_TRUE(atPlusOne.isMarginal());
    EXPECT_FALSE(atPlusOne.isStable());
    EXPECT_TRUE(atMinusOne.isMarginal());
    EXPECT_FALSE(atMinusOne.isStable());
    EXPECT_NEAR(1.0, atPlusOne.maxModulus(), 1e-12);
}

// Анализ второй модели (Model 1.3) по корням характеристического уравнения.
TEST(StabilityAnalyzerTest, SecondOrderModelRootsMatchEquation) {
    const auto result = StabilityAnalyzer::analyzeSecondOrder(0.5, 0.1);

    ASSERT_EQ(2u, result.roots.size());
    const double sq = std::sqrt(0.65);
    EXPECT_NEAR((0.5 + sq) / 2.0, result.roots[0].real(), 1e-12);
    EXPECT_NEAR((0.5 - sq) / 2.0, result.roots[1].real(), 1e-12);
    EXPECT_TRUE(result.isStable());
}

// Заведомо устойчивая Model 1.3 (базовые коэффициенты ЛР №1).
TEST(StabilityAnalyzerTest, SecondOrderModelWithBaseCoefficientsIsStable) {
    const auto result = StabilityAnalyzer::analyzeSecondOrder(0.5, 0.1);

    EXPECT_TRUE(result.isStable());
    EXPECT_LT(result.maxModulus(), 1.0);
}

// Заведомо неустойчивая Model 1.3: a2 = 2 => один из корней по модулю > 1.
TEST(StabilityAnalyzerTest, SecondOrderModelWithLargeA2IsUnstable) {
    const auto result = StabilityAnalyzer::analyzeSecondOrder(0.5, 2.0);

    EXPECT_TRUE(result.isUnstable());
    EXPECT_FALSE(result.isStable());
}

// Граница устойчивости для Model 1.3: a2 = 1 => корни z = +/-1.
TEST(StabilityAnalyzerTest, SecondOrderModelOnUnitCircleIsMarginal) {
    const auto result = StabilityAnalyzer::analyzeSecondOrder(0.0, 1.0);

    EXPECT_TRUE(result.isMarginal());
    EXPECT_NEAR(1.0, result.maxModulus(), 1e-12);
}

// Комплексно-сопряжённые корни на границе: a1 = 0, a2 = -1 => z = +/-j.
TEST(StabilityAnalyzerTest, SecondOrderModelWithComplexBoundaryIsMarginal) {
    const auto result = StabilityAnalyzer::analyzeSecondOrder(0.0, -1.0);

    EXPECT_TRUE(result.isMarginal());
    EXPECT_NEAR(1.0, result.maxModulus(), 1e-12);
}

// Аналитический критерий Джури совпадает с корневым анализом.
TEST(StabilityAnalyzerTest, JuryCriterionAgreesWithRootAnalysis) {
    const std::vector<double> coeffs = {-0.9, -0.5, -0.1, 0.0, 0.1, 0.5, 0.9, 1.0, 2.0};

    for (double a1 : coeffs) {
        for (double a2 : coeffs) {
            const auto result = StabilityAnalyzer::analyzeSecondOrder(a1, a2);
            // p1 = -a1, p0 = -a2 для уравнения z^2 - a1*z - a2 = 0.
            const bool jury = StabilityAnalyzer::jurySecondOrder(-a1, -a2);

            EXPECT_EQ(jury, result.isStable())
                << "a1 = " << a1 << ", a2 = " << a2;
        }
    }
}

// Критерий Джури на известных примерах.
TEST(StabilityAnalyzerTest, JuryCriterionKnownCases) {
    // z^2 - 0.25 = 0 => корни +/-0.5 — устойчиво.
    EXPECT_TRUE(StabilityAnalyzer::jurySecondOrder(0.0, -0.25));
    // z^2 - 1 = 0 => корни +/-1 — на границе.
    EXPECT_FALSE(StabilityAnalyzer::jurySecondOrder(0.0, -1.0));
    // z^2 + 2 = 0 => корни +/-j*sqrt(2) — неустойчиво.
    EXPECT_FALSE(StabilityAnalyzer::jurySecondOrder(0.0, 2.0));
}

// Модель Эйлера (Model 3.7): полюс z = 1 - dt*exp(a).
TEST(StabilityAnalyzerTest, EulerModelPoleMatchesDiscretization) {
    const auto result = StabilityAnalyzer::analyzeEuler(0.1, 0.1);

    ASSERT_EQ(1u, result.roots.size());
    EXPECT_NEAR(1.0 - 0.1 * std::exp(0.1), result.roots[0].real(), 1e-12);
    EXPECT_TRUE(result.isStable());
}

// Неустойчивая дискретизация: dt*exp(a) > 2.
TEST(StabilityAnalyzerTest, EulerModelWithLargeStepIsUnstable) {
    const auto result = StabilityAnalyzer::analyzeEuler(0.0, 3.0);

    EXPECT_TRUE(result.isUnstable());
    EXPECT_NEAR(-2.0, result.roots[0].real(), 1e-12);
}

// Граница устойчивости: dt*exp(a) = 2 => полюс z = -1.
TEST(StabilityAnalyzerTest, EulerModelOnUnitCircleIsMarginal) {
    const auto result = StabilityAnalyzer::analyzeEuler(0.0, 2.0);

    EXPECT_TRUE(result.isMarginal());
    EXPECT_NEAR(-1.0, result.roots[0].real(), 1e-12);
}

// Полиморфный анализ: модуль работает с любой моделью через интерфейс Model.
TEST(StabilityAnalyzerTest, PolymorphicAnalysisOfAllModels) {
    LinearModel11 stable11(0.5, 1.0);
    LinearModel13 stable13(0.5, 0.1, 1.0);
    NonlinearModel22 stable22(0.7, 0.3);
    NonlinearModel23 stable23(0.7, 0.3);
    NonlinearModel25 stable25(0.8, 0.5);
    ODEModel37 stable37(0.1, 1.0, 0.1);

    EXPECT_TRUE(StabilityAnalyzer::analyze(stable11).isStable());
    EXPECT_TRUE(StabilityAnalyzer::analyze(stable13).isStable());
    EXPECT_TRUE(StabilityAnalyzer::analyze(stable22).isStable());
    EXPECT_TRUE(StabilityAnalyzer::analyze(stable23).isStable());
    EXPECT_TRUE(StabilityAnalyzer::analyze(stable25).isStable());
    EXPECT_TRUE(StabilityAnalyzer::analyze(stable37).isStable());

    LinearModel11 unstable11(1.2, 1.0);
    LinearModel13 unstable13(0.5, 2.0, 1.0);
    NonlinearModel25 unstable25(1.05, 0.5);
    ODEModel37 unstable37(0.0, 1.0, 3.0);

    EXPECT_TRUE(StabilityAnalyzer::analyze(unstable11).isUnstable());
    EXPECT_TRUE(StabilityAnalyzer::analyze(unstable13).isUnstable());
    EXPECT_TRUE(StabilityAnalyzer::analyze(unstable25).isUnstable());
    EXPECT_TRUE(StabilityAnalyzer::analyze(unstable37).isUnstable());
}

// Проверки принадлежности модулей единичной окружности.
TEST(StabilityAnalyzerTest, UnitCircleHelpers) {
    EXPECT_TRUE(StabilityAnalyzer::allInsideUnitCircle({0.5, 0.9}));
    EXPECT_FALSE(StabilityAnalyzer::allInsideUnitCircle({0.5, 1.0}));
    EXPECT_FALSE(StabilityAnalyzer::allInsideUnitCircle({1.2}));

    EXPECT_TRUE(StabilityAnalyzer::allOnOrInsideUnitCircle({0.5, 1.0}));
    EXPECT_FALSE(StabilityAnalyzer::allOnOrInsideUnitCircle({1.2}));
    EXPECT_TRUE(StabilityAnalyzer::allOnOrInsideUnitCircle({}));
}

// Устойчивая модель: предупреждение не выводится.
TEST(StabilityAnalyzerTest, NoWarningForStableModel) {
    LinearModel11 model(0.5, 1.0);
    std::ostringstream out;

    const bool warned = StabilityAnalyzer::warnIfUnstable(model, out);

    EXPECT_FALSE(warned);
    EXPECT_TRUE(out.str().empty());
}

// Неустойчивая модель: выводится предупреждение с информацией о полюсах.
TEST(StabilityAnalyzerTest, WarningIsProducedForUnstableModel) {
    LinearModel11 model(1.5, 1.0);
    std::ostringstream out;

    const bool warned = StabilityAnalyzer::warnIfUnstable(model, out);

    EXPECT_TRUE(warned);
    EXPECT_NE(std::string::npos, out.str().find("ВНИМАНИЕ"));
    EXPECT_NE(std::string::npos, out.str().find("неустойчив"));
}

// Граничный случай: полюс на единичной окружности также требует предупреждения.
TEST(StabilityAnalyzerTest, WarningIsProducedForMarginalModel) {
    LinearModel13 model(0.0, 1.0, 1.0);
    std::ostringstream out;

    const bool warned = StabilityAnalyzer::warnIfUnstable(model, out);

    EXPECT_TRUE(warned);
    EXPECT_NE(std::string::npos, out.str().find("ВНИМАНИЕ"));
    EXPECT_NE(std::string::npos, out.str().find("границе устойчивости"));
}

// Текст предупреждения содержит сводку по полюсам.
TEST(StabilityAnalyzerTest, WarningMessageContainsPoleSummary) {
    const auto result = StabilityAnalyzer::analyzeSecondOrder(0.5, 2.0);
    const std::string message = StabilityAnalyzer::warningMessage(result);

    EXPECT_NE(std::string::npos, message.find("полюса"));
    EXPECT_NE(std::string::npos, message.find("|z|"));
}

// Отчёт об устойчивости содержит имя модели, уравнение и вывод.
TEST(StabilityAnalyzerTest, FormatReportContainsModelInformation) {
    LinearModel13 model(0.5, 0.1, 1.0);
    const std::string report = StabilityAnalyzer::formatReport(model);

    EXPECT_NE(std::string::npos, report.find("Model 1.3"));
    EXPECT_NE(std::string::npos, report.find("y(tau+1)"));
    EXPECT_NE(std::string::npos, report.find("устойчива"));
    EXPECT_NE(std::string::npos, report.find("|z|"));
}

// Сводка результата анализа содержит модули полюсов.
TEST(StabilityAnalyzerTest, ResultSummaryContainsModuli) {
    const auto result = StabilityAnalyzer::analyzeFirstOrder(0.25);
    const std::string summary = result.summary();

    EXPECT_NE(std::string::npos, summary.find("устойчива"));
    EXPECT_NE(std::string::npos, summary.find("0.2500"));
}

// Максимальный модуль вычисляется по всем полюсам.
TEST(StabilityAnalyzerTest, MaxModulusTakesLargestPole) {
    const auto result = StabilityAnalyzer::analyzeSecondOrder(0.0, -0.25);

    ASSERT_EQ(2u, result.moduli.size());
    EXPECT_NEAR(0.5, result.maxModulus(), 1e-12);
}

}  // namespace
