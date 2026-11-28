#ifndef TASK02_AS06818_STABILITY_ANALYZER_HPP
#define TASK02_AS06818_STABILITY_ANALYZER_HPP

#include <complex>
#include <string>

#include "linear_model.hpp"

// Результат анализа устойчивости линейной дискретной модели 2-го порядка
// (Model 1.8) по корням характеристического уравнения на Z-плоскости.
struct StabilityReport {
    bool stable;                 // true, если оба корня |z| < 1
    std::complex<double> z1;     // первый корень z^2 - a1*z - a2 = 0
    std::complex<double> z2;     // второй корень
    double spectral_radius;      // max(|z1|, |z2|) — спектральный радиус
};

// Анализ устойчивости по коэффициентам знаменателя:
// характеристическое уравнение z^2 - a1*z - a2 = 0
// (однородная часть y_[tau+1] = a1*y_tau + a2*y_[tau-1]).
// Критерий: процесс сходится тогда и только тогда, когда оба корня
// лежат строго внутри единичной окружности Z-плоскости.
StabilityReport analyze_stability(double a1, double a2);

// Анализ устойчивости конкретного экземпляра модели 1.8.
StabilityReport analyze_stability(const LinearModel18& model);

// Текст предупреждения для вывода в консоль перед симуляцией
// неустойчивого процесса (пустая строка, если процесс устойчив).
std::string stability_warning(const StabilityReport& report);

#endif
