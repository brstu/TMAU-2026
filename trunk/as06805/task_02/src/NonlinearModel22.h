#pragma once

#include "Model.h"

// Нелинейная модель с насыщением исполнительного механизма:
//   y(tau+1) = a * y(tau) + b * sat(u(tau))
//   sat(u) = Umax,            если u >  Umax
//          = u,                если Umin <= u <= Umax
//          = Umin,            если u <  Umin
// Однородная часть: y(tau+1) = a * y(tau)  =>  полюс z = a.
class NonlinearModel22 : public Model {
public:
    NonlinearModel22(double a, double b, double uMin = -1.0, double uMax = 1.0);

    double step(double u) override;
    void reset() override;
    std::string name() const override;
    std::vector<std::complex<double>> poles() const override;
    std::string equation() const override;
    void setInitialState(const std::vector<double>& init) override;

    // Функция насыщения (public — используется в тестах граничных значений).
    double saturate(double u) const;

    double a() const noexcept { return a_; }
    double b() const noexcept { return b_; }
    double uMin() const noexcept { return uMin_; }
    double uMax() const noexcept { return uMax_; }

private:
    double a_;
    double b_;
    double uMin_;
    double uMax_;
    double y_ = 0.0;
};
