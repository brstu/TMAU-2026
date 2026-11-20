#ifndef TASK03_AS06818_NONLINEAR_MODEL_HPP
#define TASK03_AS06818_NONLINEAR_MODEL_HPP

#include "model.hpp"

/// @file nonlinear_model.hpp
/// Нелинейная модель объекта 2.2 (вариант 18, ЛР1).

/// Нелинейная модель 2.2 — с насыщением исполнительного механизма:
/// y[t+1] = a*y[t] + b*sat(u[t]),
/// sat(u) = u_max при u > u_max; u при u_min <= u <= u_max; u_min при u < u_min.
class NonlinearModel22 : public Model {
public:
    /// Задаёт коэффициенты a, b, пределы насыщения и начальное y[0].
    explicit NonlinearModel22(double a = 0.7, double b = 0.5,
                              double u_min = -0.5, double u_max = 0.5,
                              double y_start = 0.0);

    double step(double u) override;
    void reset() override;
    double get_y() const override;

    /// Статическая характеристика звена насыщения.
    double sat(double u) const;

private:
    double a;
    double b;
    double u_min;
    double u_max;
    double y_start;
    double y_curr;
};

#endif
