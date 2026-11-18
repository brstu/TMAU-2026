#include "stability_analyzer.h"
#include <iostream>
#include <cmath>
#include <algorithm>

std::vector<std::complex<double>> StabilityAnalyzer::getRootsLinearModel1_4(double a) {
    // Для уравнения первого порядка y_{t+1} = a*y_t + ...
    // Характеристическое уравнение: z - a = 0 => z = a
    return { std::complex<double>(a, 0.0) };
}

bool StabilityAnalyzer::isStableLinearModel1_4(double a) {
    auto roots = getRootsLinearModel1_4(a);
    return std::none_of(roots.begin(), roots.end(), [](const auto& root) {
        return std::abs(root) >= 1.0;
    });
}

bool StabilityAnalyzer::checkAndWarnStability(double a) {
    bool stable = isStableLinearModel1_4(a);
    if (!stable) {
        std::cout << "\n======================================================\n";
        std::cout << "[ПРЕДУПРЕЖДЕНИЕ] ВНИМАНИЕ: Выбранный коэффициент a = " << a 
                  << " приводит к неустойчивому (расходящемуся) процессу!\n";
        std::cout << "Корень характеристического уравнения |z| = " << std::abs(a) << " >= 1.0\n";
        std::cout << "======================================================\n\n";
    }
    return stable;
}