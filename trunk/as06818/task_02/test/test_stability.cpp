#include <gtest/gtest.h>

#include <string>

#include "../src/linear_model.hpp"
#include "../src/stability_analyzer.hpp"

// Коэффициенты по умолчанию (a1=0.5, a2=0.2):
//   z^2 - 0.5z - 0.2 = 0, D = 0.25 + 0.8 = 1.05,
//   z1 = (0.5 + sqrt(1.05))/2 = 0.762348..., z2 = -0.262348...
TEST(StabilityTest, DefaultCoefficientsStable) {
    const StabilityReport report = analyze_stability(0.5, 0.2);

    EXPECT_TRUE(report.stable);
    EXPECT_NEAR(report.spectral_radius, 0.762347538297980, 1e-9);
    EXPECT_NEAR(report.z1.real(), 0.762347538297980, 1e-9);
    EXPECT_NEAR(report.z2.real(), -0.262347538297980, 1e-9);
    EXPECT_DOUBLE_EQ(report.z1.imag(), 0.0);
}

// Граница устойчивости |z| = 1: z^2 - 0.5z - 0.5 = (z - 1)(z + 0.5).
// Модуль корня равен единице => процесс не сходится (требуется |z| < 1).
TEST(StabilityTest, BoundaryRootOnUnitCircleIsUnstable) {
    const StabilityReport report = analyze_stability(0.5, 0.5);

    EXPECT_FALSE(report.stable);
    EXPECT_DOUBLE_EQ(report.spectral_radius, 1.0);
    EXPECT_DOUBLE_EQ(report.z1.real(), 1.0);
    EXPECT_DOUBLE_EQ(report.z2.real(), -0.5);
}

// Расходящийся процесс: z^2 - 1.5z - 0.5 = 0 => z1 = 1.780776...
TEST(StabilityTest, DivergentCoefficientsUnstable) {
    const StabilityReport report = analyze_stability(1.5, 0.5);

    EXPECT_FALSE(report.stable);
    EXPECT_NEAR(report.spectral_radius, 1.780776406404415, 1e-9);
}

// Комплексные корни: z^2 + 0.5 = 0 => z = +-0.707107i, оба внутри
// единичной окружности.
TEST(StabilityTest, ComplexRootsInsideUnitCircle) {
    const StabilityReport report = analyze_stability(0.0, -0.5);

    EXPECT_TRUE(report.stable);
    EXPECT_NEAR(report.spectral_radius, 0.707106781186548, 1e-9);
    EXPECT_NEAR(report.z1.imag(), 0.707106781186548, 1e-9);
    EXPECT_NEAR(report.z2.imag(), -0.707106781186548, 1e-9);
    EXPECT_DOUBLE_EQ(report.z1.real(), 0.0);
}

// Вырожденный случай a2 = 0: z^2 - 0.8z = z(z - 0.8) => z1 = 0.8, z2 = 0.
TEST(StabilityTest, DegenerateFirstOrderCase) {
    const StabilityReport report = analyze_stability(0.8, 0.0);

    EXPECT_TRUE(report.stable);
    EXPECT_DOUBLE_EQ(report.z1.real(), 0.8);
    EXPECT_DOUBLE_EQ(report.z2.real(), 0.0);
}

// Анализ через экземпляр модели: коэффициенты берутся из самой модели.
TEST(StabilityTest, OverloadForLinearModel) {
    const LinearModel18 stable_model(0.5, 0.2, 0.3, 0.1);
    EXPECT_TRUE(analyze_stability(stable_model).stable);

    const LinearModel18 unstable_model(1.5, 0.5, 0.3, 0.1);
    EXPECT_FALSE(analyze_stability(unstable_model).stable);
}

// Для устойчивого процесса предупреждение пустое, для неустойчивого
// содержит ключевые слова.
TEST(StabilityWarningTest, WarningText) {
    const StabilityReport stable = analyze_stability(0.5, 0.2);
    EXPECT_TRUE(stability_warning(stable).empty());

    const StabilityReport unstable = analyze_stability(1.5, 0.5);
    const std::string warning = stability_warning(unstable);
    EXPECT_NE(warning.find("ПРЕДУПРЕЖДЕНИЕ"), std::string::npos);
    EXPECT_NE(warning.find("1.780776"), std::string::npos);
}
