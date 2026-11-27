#pragma once
#include "Model.h"

// Модель 2.3 (вариант 19).
// Формула: y[k+1] = a1*y[k] - b1*(y[k-1])^2 + c1*u[k]
class NonlinearModel23 : public Model {
public:
    NonlinearModel23(double a1, double b1, double c1)
        : a1_(a1), b1_(b1), c1_(c1) {}

    double step(double u) override {
        const double y0 = y_curr_;
        y_curr_ = a1_ * y0 - b1_ * (y_prev1_ * y_prev1_) + c1_ * u;
        y_prev1_ = y0;
        return y_curr_;
    }

    void reset() override {
        y_curr_ = 0.0;
        y_prev1_ = 0.0;
    }

    const char* name() const override { return "Model_2_3"; }

private:
    double a1_;
    double b1_;
    double c1_;
    double y_curr_  = 0.0;
    double y_prev1_ = 0.0;
};