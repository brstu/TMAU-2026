#pragma once

#include <complex>
#include <string>
#include <vector>

class IModel;

struct StabilityReport {
    bool analyzed = false;
    bool stable = true;
    bool marginal = false;
    double maxMagnitude = 0.0;
    std::vector<std::complex<double>> roots;
    std::string message;
};

// Анализ устойчивости линейных дискретных систем по корням
// характеристического уравнения на Z-плоскости.
class StabilityAnalyzer {
public:
    static constexpr double kTolerance = 1e-7;

    // Корни полинома a0*z^n + a1*z^(n-1) + ... + an (метод Дюрана - Кернера).
    static std::vector<std::complex<double>> findRoots(
        const std::vector<double>& coefficients);

    static StabilityReport analyze(const std::vector<double>& coefficients);
    static StabilityReport analyze(const IModel& model);

    static std::string buildWarning(const IModel& model,
                                    const StabilityReport& report);
};
