#ifndef STABILITY_ANALYZER_H
#define STABILITY_ANALYZER_H

#include <complex>
#include <ostream>
#include <stdexcept>
#include <string>
#include <vector>

// Модуль анализа устойчивости дискретных моделей варианта 15.
// Устойчивость определяется по корням характеристического уравнения
// на Z-плоскости: все корни должны лежать строго внутри единичной окружности.

enum class Stability {
    Stable,    // max|z| < 1  - процесс затухает
    Boundary,  // max|z| = 1  - процесс не затухает (граница устойчивости)
    Unstable   // max|z| > 1  - процесс расходится
};

// Результат анализа одной модели
struct StabilityResult {
    Stability verdict = Stability::Unstable;
    double maxModulus = 0.0;  // наибольший модуль корня
    std::string equation;     // характеристическое уравнение в текстовом виде
};

// Допуск, с которым корень считается лежащим на окружности |z| = 1
constexpr double kBoundaryTolerance = 1e-7;

// Исключение, которое бросает requireStable() для неустойчивой модели
class UnstableSystemError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

// Все корни полинома со старшим коэффициентом 1 (метод Дюрана-Кернера).
// Коэффициенты перечислены от старшей степени к младшей.
std::vector<std::complex<double>> polynomialRoots(const std::vector<double>& coeffs);

// Классификация по наибольшему модулю корня
Stability classify(double maxModulus);

// Model 1.5: z^(k+1) - a1*z^k - a2 = 0
StabilityResult analyzeModel1_5(double a1, double a2, int k);

// Model 2.9: z - a = 0 (нелинейность действует только на вход)
StabilityResult analyzeModel2_9(double a);

// Model 3.3, схема Эйлера: z - (1 - a*dt) = 0
StabilityResult analyzeModel3_3(double a, double dt);

// Аналитический критерий Джури для Model 1.5 при k = 1:
// z^2 - a1*z - a2 = 0 устойчиво <=> |a2| < 1, 1 - a1 - a2 > 0, 1 + a1 - a2 > 0
bool juryCriterionK1(double a1, double a2);

// true только для асимптотически устойчивой модели
bool isStable(const StabilityResult& result);

// Печатает предупреждение, если модель не устойчива. Возвращает true, если предупреждение было.
bool warnIfUnstable(const std::string& modelName, const StabilityResult& result, std::ostream& out);

// Бросает UnstableSystemError, если модель не устойчива
void requireStable(const std::string& modelName, const StabilityResult& result);

#endif // STABILITY_ANALYZER_H
