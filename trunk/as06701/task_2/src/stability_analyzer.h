#pragma once
#include <vector>
#include <complex>
#include <string>

/** @brief Результат анализа устойчивости. */
struct StabilityResult {
    bool stable = false;
    std::string reason;                          ///< Человекочитаемое объяснение
    std::vector<std::complex<double>> roots;     ///< Найденные корни
    double maxMagnitude = 0.0;                   ///< max |z_i|
};

/**
 * @brief Анализ устойчивости линейных дискретных моделей.
 *
 * Коэффициенты полинома передаются в порядке возрастания степени:
 *   coeffs[i] — коэффициент при z^i; старший коэффициент coeffs.back() != 0.
 */
class StabilityAnalyzer {
public:
    /** Найти все корни полинома (метод Дюрана–Кернера). */
    static std::vector<std::complex<double>> findRoots(const std::vector<double>& coeffs);

    /** Проверить устойчивость: все |z| < 1 (с допуском). */
    static StabilityResult analyze(const std::vector<double>& coeffs);

    /** Model 1.5: z^{k+1} − a1·z^k − a2 (или z − (a1+a2) при k=0). */
    static StabilityResult analyzeModel15(double a1, double a2, int k);

    /** Model 2.7: z − a. */
    static StabilityResult analyzeModel27(double a);

    /** Model 3.9: z − 1 (маргинально устойчива). */
    static StabilityResult analyzeModel39();
};
