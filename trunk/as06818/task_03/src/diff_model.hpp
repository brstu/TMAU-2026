#ifndef TASK03_AS06818_DIFF_MODEL_HPP
#define TASK03_AS06818_DIFF_MODEL_HPP

#include "model.hpp"

/// @file diff_model.hpp
/// Дифференциальное уравнение 3.6 (вариант 18, ЛР1).

/// Модель 3.6 — дифференциальное уравнение кубического роста с управлением:
/// dy/dt = a*y^3 + b*u.
/// Интегрируется явным методом Эйлера с шагом dt:
/// y[t+1] = y[t] + dt * (a*y[t]^3 + b*u[t]).
class DiffModel36 : public Model {
public:
    /// Задаёт коэффициенты a, b, шаг интегрирования dt и начальное y(0).
    explicit DiffModel36(double a = -0.1, double b = 0.5,
                         double dt = 0.1, double y_start = 0.0);

    double step(double u) override;
    void reset() override;
    double get_y() const override;

    /// Шаг интегрирования dt.
    double get_dt() const;

private:
    double a;
    double b;
    double dt;
    double y_start;
    double y_curr;
};

#endif
