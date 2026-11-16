#ifndef TASK02_AS06818_DIFF_MODEL_HPP
#define TASK02_AS06818_DIFF_MODEL_HPP

#include "model.hpp"

// Model 3.6 — дифференциальное уравнение кубического роста с управлением:
//   dy/dt = a*y^3 + b*u,
// решается явным методом Эйлера с шагом dt:
//   y_[tau+1] = y_tau + dt * (a*y_tau^3 + b*u_tau).
class DiffModel36 : public Model {
public:
    explicit DiffModel36(double a = -0.1, double b = 0.5,
                         double dt = 0.1, double y_start = 0.0);

    double step(double u) override;
    void reset() override;

    double get_y() const;

private:
    double a;
    double b;
    double dt;
    double y_start;
    double y_curr;
};

#endif
