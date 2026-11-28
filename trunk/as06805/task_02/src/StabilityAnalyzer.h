#pragma once

#include "Model.h"

#include <complex>
#include <iosfwd>
#include <string>
#include <vector>

// Модуль анализа устойчивости дискретных систем.
//
// Для линейной дискретной модели y(tau+1) = sum(a_i * y(tau-i)) + b * u(tau)
// характеристическое уравнение имеет вид
//       z^n - a1*z^(n-1) - a2*z^(n-2) - ... - an = 0.
// Процесс асимптотически устойчив тогда и только тогда, когда все корни
// (полюса) лежат внутри единичной окружности Z-плоскости: |z| < 1.
// Если хотя бы один полюс лежит на окружности |z| = 1 — система на границе
// устойчивости ( marginally stable ), если |z| > 1 — процесс расходится.
class StabilityAnalyzer {
public:
    enum class Status {
        Stable,    // все полюса внутри единичной окружности
        Marginal,  // полюса на границе |z| = 1
        Unstable   // есть полюса вне единичной окружности
    };

    struct Result {
        Status status = Status::Stable;
        std::vector<std::complex<double>> roots;  // полюса на Z-плоскости
        std::vector<double> moduli;               // модули полюсов |z|

        bool isStable() const noexcept { return status == Status::Stable; }
        bool isMarginal() const noexcept { return status == Status::Marginal; }
        bool isUnstable() const noexcept { return status == Status::Unstable; }

        // Максимальный модуль полюса.
        double maxModulus() const noexcept;

        // Текстовая сводка по результатам анализа.
        std::string summary() const;
    };

    // Допуск при сравнении модуля полюса с единицей (граница |z| = 1).
    static constexpr double kEps = 1e-9;

    // Полиморфный анализ любой модели: полюса берутся у самой модели.
    static Result analyze(const Model& model);

    // Модель первого порядка: y(tau+1) = a*y(tau) + b*u(tau)  =>  полюс z = a.
    static Result analyzeFirstOrder(double a);

    // Модель второго порядка (Model 1.3):
    //   y(tau+1) = a1*y(tau) + a2*y(tau-1) + b*u(tau),
    //   z^2 - a1*z - a2 = 0  =>  z = (a1 +/- sqrt(a1^2 + 4*a2)) / 2.
    static Result analyzeSecondOrder(double a1, double a2);

    // Непрерывная модель, дискретизированная методом Эйлера (Model 3.7):
    //   dy/dt = -exp(a)*y + b*u  =>  дискретный полюс z = 1 - dt*exp(a).
    static Result analyzeEuler(double a, double dt);

    // Аналитический критерий Джури для уравнения z^2 + p1*z + p0 = 0:
    //   |p0| < 1,  1 + p1 + p0 > 0,  1 - p1 + p0 > 0.
    static bool jurySecondOrder(double p1, double p0, double eps = kEps);

    // Проверка, что все модули строго меньше единицы.
    static bool allInsideUnitCircle(const std::vector<double>& moduli, double eps = kEps);

    // Проверка, что все модули не больше единицы (нет полюсов вне круга).
    static bool allOnOrInsideUnitCircle(const std::vector<double>& moduli, double eps = kEps);

    // Формирование текстового предупреждения о неустойчивости.
    static std::string warningMessage(const Result& result);

    // Вывод предупреждения в поток (по умолчанию — в консоль).
    // Возвращает true, если процесс признан неустойчивым (или граничным).
    static bool warnIfUnstable(const Model& model, std::ostream& os);

    // Полный отчёт об устойчивости модели (для вывода в консоль).
    static std::string formatReport(const Model& model);
};
