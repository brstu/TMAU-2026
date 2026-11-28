#pragma once

#include "Model.h"

// Нелинейная модель с трением и экспоненциальным насыщением:
//   y(tau+1) = a * y(tau) + b * sign(u(tau)) * (1 - exp(-|u(tau)|))
// Однородная часть: y(tau+1) = a * y(tau)  =>  полюс z = a.
class NonlinearModel25 : public Model {
public:
    NonlinearModel25(double a, double b);

    double step(double u) override;
    void reset() override;
    std::string name() const override;
    std::vector<std::complex<double>> poles() const override;
    std::string equation() const override;
    void setInitialState(const std::vector<double>& init) override;

    // Знаковая экспоненциальная нелинейность (public — используется в тестах).
    double friction(double u) const;

    double a() const noexcept { return a_; }
    double b() const noexcept { return b_; }

private:
    double a_;
    double b_;
    double y_ = 0.0;
};
