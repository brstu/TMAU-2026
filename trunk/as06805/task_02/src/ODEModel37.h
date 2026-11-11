#pragma once

#include "Model.h"

// Непрерывное дифференциальное уравнение, решаемое методом Эйлера:
//   dy/dt = -exp(a) * y + b * u
//   =>  y(tau+1) = y(tau) + dt * (-exp(a) * y(tau) + b * u(tau))
// Дискретный полюс (после дискретизации): z = 1 - dt * exp(a)
class ODEModel37 : public Model {
public:
    ODEModel37(double a, double b, double dt);

    double step(double u) override;
    void reset() override;
    std::string name() const override;
    std::vector<std::complex<double>> poles() const override;
    std::string equation() const override;
    void setInitialState(const std::vector<double>& init) override;

    double a() const noexcept { return a_; }
    double b() const noexcept { return b_; }
    double dt() const noexcept { return dt_; }

    // Непрерывный коэффициент затухания exp(a).
    double decay() const;

private:
    double a_;
    double b_;
    double dt_;
    double y_ = 0.0;
};
