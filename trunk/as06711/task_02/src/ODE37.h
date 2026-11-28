#pragma once
#include "Model.h"
#include <cmath>

// Модель 3.7 (вариант 19), метод Эйлера:
// dy/dt = -e^a*y + b*u  ==>  y[k+1] = y[k] + dt*(-e^a*y[k] + b*u[k])
class ODE37 : public Model {
public:
    ODE37(double a, double b, double dt)
        : a_(a), b_(b), dt_(dt) {}

    double step(double u) override {
        y_curr_ = y_curr_ + dt_ * (-std::exp(a_) * y_curr_ + b_ * u);
        return y_curr_;
    }

    void reset() override { y_curr_ = 0.0; }

    const char* name() const override { return "Model_3_7"; }

private:
    double a_;
    double b_;
    double dt_;
    double y_curr_ = 0.0;
};
