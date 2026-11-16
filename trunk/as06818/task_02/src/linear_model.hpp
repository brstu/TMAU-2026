#ifndef TASK02_AS06818_LINEAR_MODEL_HPP
#define TASK02_AS06818_LINEAR_MODEL_HPP

#include "model.hpp"

// Model 1.8 — обобщённая авторегрессионная линейная модель:
//   y_[tau+1] = a1*y_tau + a2*y_[tau-1] + b1*u_tau + b2*u_[tau-1]
// Начальные условия: y_0 = y_start, y_-1 = 0, u_-1 = 0.
class LinearModel18 : public Model {
public:
    explicit LinearModel18(double a1 = 0.5, double a2 = 0.2,
                           double b1 = 0.3, double b2 = 0.1,
                           double y_start = 0.0);

    double step(double u) override;
    void reset() override;

    // Коэффициенты знаменателя передаточной функции (для анализа устойчивости).
    double get_a1() const;
    double get_a2() const;

    // Текущее значение выходной величины.
    double get_y() const;

private:
    double a1;
    double a2;
    double b1;
    double b2;
    double y_start;
    double y_curr;
    double y_prev;
    double u_prev;
};

#endif
