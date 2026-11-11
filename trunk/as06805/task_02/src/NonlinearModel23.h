#pragma once

#include "Model.h"

// Нелинейная модель с зоной нечувствительности (dead zone):
//   y(tau+1) = a * y(tau) + b * dz(u(tau))
//   dz(u) = u - delta,   если u >  delta
//        = 0,           если -delta <= u <= delta
//        = u + delta,   если u < -delta
// Однородная часть: y(tau+1) = a * y(tau)  =>  полюс z = a.
class NonlinearModel23 : public Model {
public:
    NonlinearModel23(double a, double b, double delta = 0.1);

    double step(double u) override;
    void reset() override;
    std::string name() const override;
    std::vector<std::complex<double>> poles() const override;
    std::string equation() const override;
    void setInitialState(const std::vector<double>& init) override;

    // Функция зоны нечувствительности (public — используется в тестах).
    double deadZone(double u) const;

    double a() const noexcept { return a_; }
    double b() const noexcept { return b_; }
    double delta() const noexcept { return delta_; }

private:
    double a_;
    double b_;
    double delta_;
    double y_ = 0.0;
};
