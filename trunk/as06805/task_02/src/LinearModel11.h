#pragma once

#include "Model.h"

// Линейная модель первого порядка (инерционное звено):
//   y(tau+1) = a * y(tau) + b * u(tau)
// Характеристическое уравнение: z - a = 0  =>  полюс z = a.
class LinearModel11 : public Model {
public:
    LinearModel11(double a, double b);

    double step(double u) override;
    void reset() override;
    std::string name() const override;
    std::vector<std::complex<double>> poles() const override;
    std::string equation() const override;
    void setInitialState(const std::vector<double>& init) override;

    double a() const noexcept { return a_; }
    double b() const noexcept { return b_; }

private:
    double a_;
    double b_;
    double y_ = 0.0;
};
