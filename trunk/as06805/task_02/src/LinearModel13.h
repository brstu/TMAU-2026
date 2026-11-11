#pragma once

#include "Model.h"

// Линейная модель второго порядка (инерционное колебательное звено):
//   y(tau+1) = a1 * y(tau) + a2 * y(tau-1) + b * u(tau)
// Характеристическое уравнение: z^2 - a1*z - a2 = 0
//   =>  z1,2 = (a1 +/- sqrt(a1^2 + 4*a2)) / 2
class LinearModel13 : public Model {
public:
    LinearModel13(double a1, double a2, double b);

    double step(double u) override;
    void reset() override;
    std::string name() const override;
    std::vector<std::complex<double>> poles() const override;
    std::string equation() const override;
    void setInitialState(const std::vector<double>& init) override;

    double a1() const noexcept { return a1_; }
    double a2() const noexcept { return a2_; }
    double b() const noexcept { return b_; }

private:
    double a1_;
    double a2_;
    double b_;
    double yCurr_ = 0.0;
    double yPrev_ = 0.0;
};
