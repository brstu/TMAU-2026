#include <gtest/gtest.h>

#include <algorithm>
#include <cmath>
#include <limits>
#include <sstream>
#include <ostream>
#include <string>

#include "Model1_5.h"
#include "Model2_9.h"
#include "Model3_3.h"
#include "StabilityAnalyzer.h"

namespace {

    template <class T>
    std::string caseName(const testing::TestParamInfo<T>& info) {
        return "case" + std::to_string(info.index);
    }

    // ---------- Поиск корней полинома

    TEST(PolynomialRootsTest, FindsKnownRealRoots) {
        // (z - 1)(z - 2)(z - 3) = z^3 - 6z^2 + 11z - 6
        auto roots = polynomialRoots({ 1.0, -6.0, 11.0, -6.0 });
        ASSERT_EQ(roots.size(), 3u);
        std::sort(roots.begin(), roots.end(),
                  [](auto l, auto r) { return l.real() < r.real(); });
        for (int i = 0; i < 3; ++i) {
            EXPECT_NEAR(roots[i].real(), i + 1.0, 1e-9);
            EXPECT_NEAR(roots[i].imag(), 0.0, 1e-9);
        }
    }

    TEST(PolynomialRootsTest, FindsComplexRoots) {
        // z^2 + 1 = 0  =>  z = +-i, |z| = 1
        for (const auto& r : polynomialRoots({ 1.0, 0.0, 1.0 })) {
            EXPECT_NEAR(std::abs(r), 1.0, 1e-9);
            EXPECT_NEAR(r.real(), 0.0, 1e-9);
        }
    }

    TEST(PolynomialRootsTest, ConstantPolynomialHasNoRoots) {
        EXPECT_TRUE(polynomialRoots({ 1.0 }).empty());
        EXPECT_TRUE(polynomialRoots({}).empty());
    }

    // ---------- Классификация по |z|, проверка границы |z| = 1

    struct ClassifyCase {
        double modulus;
        Stability expected;
    };

    const char* toText(Stability s) {
        switch (s) {
        case Stability::Stable:   return "Stable";
        case Stability::Boundary: return "Boundary";
        default:                  return "Unstable";
        }
    }

    std::ostream& operator<<(std::ostream& os, const ClassifyCase& c) {
        return os << "|z|=" << c.modulus << " -> " << toText(c.expected);
    }

    class ClassifyTest : public testing::TestWithParam<ClassifyCase> {};

    TEST_P(ClassifyTest, VerdictMatchesUnitCircle) {
        EXPECT_EQ(classify(GetParam().modulus), GetParam().expected);
    }

    INSTANTIATE_TEST_SUITE_P(UnitCircle, ClassifyTest, testing::Values(
        ClassifyCase{ 0.0,         Stability::Stable },
        ClassifyCase{ 0.5,         Stability::Stable },
        ClassifyCase{ 0.999,       Stability::Stable },
        ClassifyCase{ 1.0,         Stability::Boundary },
        ClassifyCase{ 1.0 + 1e-9,  Stability::Boundary },  // погрешность численного поиска
        ClassifyCase{ 1.0 - 1e-9,  Stability::Boundary },
        ClassifyCase{ 1.001,       Stability::Unstable },
        ClassifyCase{ 5.0,         Stability::Unstable },
        ClassifyCase{ std::numeric_limits<double>::quiet_NaN(), Stability::Unstable },
        ClassifyCase{ std::numeric_limits<double>::infinity(),  Stability::Unstable }
    ), caseName<ClassifyCase>);

    // ---------- Model 1.5: z^(k+1) - a1*z^k - a2 = 0

    struct Model15Case {
        double a1;
        double a2;
        int k;
        Stability expected;
    };

    std::ostream& operator<<(std::ostream& os, const Model15Case& c) {
        return os << "a1=" << c.a1 << ", a2=" << c.a2 << ", k=" << c.k << " -> " << toText(c.expected);
    }

    class Model15StabilityTest : public testing::TestWithParam<Model15Case> {};

    TEST_P(Model15StabilityTest, VerdictMatchesRoots) {
        const auto c = GetParam();
        const StabilityResult r = analyzeModel1_5(c.a1, c.a2, c.k);
        EXPECT_EQ(r.verdict, c.expected) << r.equation << ", max|z| = " << r.maxModulus;
        EXPECT_EQ(isStable(r), c.expected == Stability::Stable);
    }

    INSTANTIATE_TEST_SUITE_P(Roots, Model15StabilityTest, testing::Values(
        Model15Case{ 0.5,  0.3, 2, Stability::Stable },    // значения по умолчанию, max|z| ~ 0.884
        Model15Case{ 0.0,  0.0, 1, Stability::Stable },    // z^2 = 0
        Model15Case{ 0.1,  0.06, 1, Stability::Stable },   // (z - 0.3)(z + 0.2)
        Model15Case{-0.4,  0.3, 2, Stability::Stable },    // |a1| + |a2| < 1
        Model15Case{ 0.5,  0.5, 1, Stability::Boundary },  // (z - 1)(z + 0.5), z = 1
        Model15Case{ 0.0,  1.0, 1, Stability::Boundary },  // z = +-1
        Model15Case{ 0.0, -1.0, 1, Stability::Boundary },  // z = +-i
        Model15Case{ 1.0,  0.0, 2, Stability::Boundary },  // z^2 (z - 1)
        Model15Case{ 1.2,  0.5, 3, Stability::Unstable },  // пример из ЛР №1, max|z| ~ 1.387
        Model15Case{ 0.6,  0.5, 1, Stability::Unstable },  // a1 + a2 > 1
        Model15Case{-0.9,  0.2, 1, Stability::Unstable },  // 1 + a1 - a2 < 0
        Model15Case{ 0.0,  1.5, 4, Stability::Unstable }
    ), caseName<Model15Case>);

    TEST(Model15StabilityTest2, MaxModulusOfKnownRoots) {
        EXPECT_NEAR(analyzeModel1_5(0.1, 0.06, 1).maxModulus, 0.3, 1e-9);
        EXPECT_NEAR(analyzeModel1_5(0.5, 0.3, 2).maxModulus, 0.884, 1e-3);
        EXPECT_NEAR(analyzeModel1_5(1.2, 0.5, 3).maxModulus, 1.3873, 1e-4);
    }

    TEST(Model15StabilityTest2, EquationTextHasCorrectDegree) {
        EXPECT_NE(analyzeModel1_5(0.5, 0.3, 2).equation.find("z^3"), std::string::npos);
        EXPECT_NE(analyzeModel1_5(0.5, 0.3, 0).equation.find("z^2"), std::string::npos);  // k < 1 -> k = 1
    }

    // Аналитический критерий Джури (k = 1) и численный поиск корней должны совпадать
    struct JuryCase {
        double a1;
        double a2;
    };

    std::ostream& operator<<(std::ostream& os, const JuryCase& c) {
        return os << "a1=" << c.a1 << ", a2=" << c.a2;
    }

    class JuryAgreementTest : public testing::TestWithParam<JuryCase> {};

    TEST_P(JuryAgreementTest, NumericRootsAgreeWithJury) {
        const auto c = GetParam();
        EXPECT_EQ(isStable(analyzeModel1_5(c.a1, c.a2, 1)), juryCriterionK1(c.a1, c.a2));
    }

    INSTANTIATE_TEST_SUITE_P(Grid, JuryAgreementTest, testing::Values(
        JuryCase{ 0.5,  0.3 }, JuryCase{ 1.5, -0.7 }, JuryCase{-1.5, -0.7 },
        JuryCase{ 0.2,  0.9 }, JuryCase{ 1.2, -0.1 }, JuryCase{ 0.0, -1.2 },
        JuryCase{ 1.9, -0.95 }, JuryCase{-0.3, 0.75 }
    ), caseName<JuryCase>);

    // ---------- Model 2.9: z = a

    struct SimpleCase {
        double a;
        double dt;
        Stability expected;
    };

    std::ostream& operator<<(std::ostream& os, const SimpleCase& c) {
        return os << "a=" << c.a << ", dt=" << c.dt << " -> " << toText(c.expected);
    }

    class Model29StabilityTest : public testing::TestWithParam<SimpleCase> {};

    TEST_P(Model29StabilityTest, RootIsA) {
        const auto c = GetParam();
        const StabilityResult r = analyzeModel2_9(c.a);
        EXPECT_EQ(r.verdict, c.expected);
        EXPECT_NEAR(r.maxModulus, std::fabs(c.a), 1e-12);
    }

    INSTANTIATE_TEST_SUITE_P(Roots, Model29StabilityTest, testing::Values(
        SimpleCase{ 0.7,  0.0, Stability::Stable },
        SimpleCase{-0.99, 0.0, Stability::Stable },
        SimpleCase{ 1.0,  0.0, Stability::Boundary },
        SimpleCase{-1.0,  0.0, Stability::Boundary },
        SimpleCase{ 1.01, 0.0, Stability::Unstable },
        SimpleCase{-2.0,  0.0, Stability::Unstable }
    ), caseName<SimpleCase>);

    // ---------- Model 3.3: z = 1 - a*dt, устойчиво при 0 < a*dt < 2

    class Model33StabilityTest : public testing::TestWithParam<SimpleCase> {};

    TEST_P(Model33StabilityTest, RootIsOneMinusADt) {
        const auto c = GetParam();
        const StabilityResult r = analyzeModel3_3(c.a, c.dt);
        EXPECT_EQ(r.verdict, c.expected);
        EXPECT_NEAR(r.maxModulus, std::fabs(1.0 - c.a * c.dt), 1e-12);
    }

    INSTANTIATE_TEST_SUITE_P(Roots, Model33StabilityTest, testing::Values(
        SimpleCase{ 1.0,  0.1, Stability::Stable },    // z = 0.9
        SimpleCase{ 15.0, 0.1, Stability::Stable },    // z = -0.5, колебательная, но затухает
        SimpleCase{ 20.0, 0.1, Stability::Boundary },  // a*dt = 2, z = -1
        SimpleCase{ 0.0,  0.1, Stability::Boundary },  // a = 0, z = 1 (интегратор)
        SimpleCase{ 25.0, 0.1, Stability::Unstable },  // слишком большой шаг, z = -1.5
        SimpleCase{-1.0,  0.1, Stability::Unstable }   // неустойчивый объект, z = 1.1
    ), caseName<SimpleCase>);

    TEST(NonFiniteTest, NanAndInfinityAreUnstable) {
        const double nan = std::numeric_limits<double>::quiet_NaN();
        const double inf = std::numeric_limits<double>::infinity();
        EXPECT_EQ(analyzeModel2_9(nan).verdict, Stability::Unstable);
        EXPECT_EQ(analyzeModel1_5(inf, 0.1, 2).verdict, Stability::Unstable);
        EXPECT_EQ(analyzeModel3_3(1.0, nan).verdict, Stability::Unstable);
    }

    // ---------- Согласованность с методами isStable() моделей из ЛР №1

    TEST(Lab1AgreementTest, ModelMethodsGiveSameVerdict) {
        std::string msg;
        EXPECT_EQ(Model1_5(0.5, 0.3, 1.0, 2).isStable(msg), isStable(analyzeModel1_5(0.5, 0.3, 2)));
        EXPECT_EQ(Model1_5(1.2, 0.5, 1.0, 3).isStable(msg), isStable(analyzeModel1_5(1.2, 0.5, 3)));
        EXPECT_EQ(Model2_9(0.7, 1.0).isStable(msg), isStable(analyzeModel2_9(0.7)));
        EXPECT_EQ(Model2_9(1.5, 1.0).isStable(msg), isStable(analyzeModel2_9(1.5)));
        EXPECT_EQ(Model3_3(1.0, 1.0, 0.1).isStable(msg), isStable(analyzeModel3_3(1.0, 0.1)));
        EXPECT_EQ(Model3_3(25.0, 1.0, 0.1).isStable(msg), isStable(analyzeModel3_3(25.0, 0.1)));
        EXPECT_FALSE(msg.empty());
    }

    // ---------- Предупреждения и исключения

    TEST(WarningTest, NoWarningWhenStable) {
        std::ostringstream out;
        EXPECT_FALSE(warnIfUnstable("Linear_1_5", analyzeModel1_5(0.5, 0.3, 2), out));
        EXPECT_TRUE(out.str().empty());
    }

    TEST(WarningTest, WarningWhenUnstable) {
        std::ostringstream out;
        EXPECT_TRUE(warnIfUnstable("Linear_1_5", analyzeModel1_5(1.2, 0.5, 3), out));
        EXPECT_NE(out.str().find("[WARNING] Linear_1_5"), std::string::npos);
        EXPECT_NE(out.str().find("DIVERGES"), std::string::npos);
    }

    TEST(WarningTest, WarningOnBoundary) {
        std::ostringstream out;
        EXPECT_TRUE(warnIfUnstable("DiffEquation_3_3", analyzeModel3_3(20.0, 0.1), out));
        EXPECT_NE(out.str().find("|z| = 1"), std::string::npos);
    }

    TEST(ExceptionTest, RequireStableThrowsOnlyForUnstable) {
        EXPECT_NO_THROW(requireStable("Nonlinear_2_9", analyzeModel2_9(0.7)));
        EXPECT_THROW(requireStable("Nonlinear_2_9", analyzeModel2_9(1.5)), UnstableSystemError);
        EXPECT_THROW(requireStable("Nonlinear_2_9", analyzeModel2_9(-1.0)), UnstableSystemError);
    }

    // ---------- Вердикт анализатора совпадает с поведением модели во времени
    // Свободное движение после единичного импульса: при устойчивости выход затухает

    double impulseTail(Model& model, int steps) {
        double y = model.calculateNext(1.0);
        for (int i = 1; i < steps; ++i) {
            y = model.calculateNext(0.0);
        }
        return std::fabs(y);
    }

    TEST(BehaviourTest, StableModelsDecay) {
        Model1_5 m15(0.5, 0.3, 1.0, 2);
        Model2_9 m29(0.7, 1.0);
        Model3_3 m33(1.0, 1.0, 0.1);
        EXPECT_LT(impulseTail(m15, 300), 1e-6);
        EXPECT_LT(impulseTail(m29, 300), 1e-6);
        EXPECT_LT(impulseTail(m33, 300), 1e-6);
    }

    TEST(BehaviourTest, UnstableModelsGrow) {
        Model1_5 m15(1.2, 0.5, 1.0, 3);
        Model2_9 m29(1.5, 1.0);
        Model3_3 m33(25.0, 1.0, 0.1);
        EXPECT_GT(impulseTail(m15, 60), 1000.0);
        EXPECT_GT(impulseTail(m29, 60), 1000.0);
        EXPECT_GT(impulseTail(m33, 60), 1000.0);
    }

} // namespace
