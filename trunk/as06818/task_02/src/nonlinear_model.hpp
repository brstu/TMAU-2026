#ifndef TASK02_AS06818_NONLINEAR_MODEL_HPP
#define TASK02_AS06818_NONLINEAR_MODEL_HPP

#include "model.hpp"

// Model 2.2 — нелинейная модель с насыщением исполнительного механизма:
//   y_[tau+1] = a*y_tau + b*sat(u_tau),
//   sat(u) = Umax при u > Umax; u при Umin <= u <= Umax; Umin при u < Umin.
class NonlinearModel22 : public Model {
public:
    explicit NonlinearModel22(double a = 0.7, double b = 0.5,
                              double u_min = -0.5, double u_max = 0.5,
                              double y_start = 0.0);

    double step(double u) override;
    void reset() override;

    // Статическая характеристика звена насыщения.
    double sat(double u) const;

    double get_y() const;

private:
    double a;
    double b;
    double u_min;
    double u_max;
    double y_start;
    double y_curr;
};

#endif
