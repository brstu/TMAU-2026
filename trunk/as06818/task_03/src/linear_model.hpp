#ifndef TASK03_AS06818_LINEAR_MODEL_HPP
#define TASK03_AS06818_LINEAR_MODEL_HPP

#include "model.hpp"

/// @file linear_model.hpp
/// Линейная модель объекта 1.8 (вариант 18, ЛР1).

/// Линейная модель 1.8 — обобщённая авторегрессионная:
/// y[t+1] = a1*y[t] + a2*y[t-1] + b1*u[t] + b2*u[t-1].
/// Начальные условия: y[0] = y_start, y[-1] = 0, u[-1] = 0.
class LinearModel18 : public Model {
public:
    /// Задаёт коэффициенты a1, a2, b1, b2 и начальное условие y[0].
    explicit LinearModel18(double a1 = 0.5, double a2 = 0.2,
                           double b1 = 0.3, double b2 = 0.1,
                           double y_start = 0.0);

    double step(double u) override;
    void reset() override;
    double get_y() const override;

    /// Коэффициент a1 знаменателя передаточной функции.
    double get_a1() const;
    /// Коэффициент a2 знаменателя передаточной функции.
    double get_a2() const;

private:
    double a1;
    double a2;
    double b1;
    double b2;
    double y_start;
    double y_curr;
    double y_prev = 0.0;
    double u_prev = 0.0;
};

#endif
