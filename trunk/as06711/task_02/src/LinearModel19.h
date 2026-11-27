#pragma once
#include "Model.h"

// Модель 1.9 (вариант 19):
// y[k+1] = a1*y[k] + a2*y[k-1] + a3*y[k-2] + b1*u[k]
class LinearModel19 : public Model {
public:
    LinearModel19(double a1, double a2, double a3, double b1)
        : a1_(a1), a2_(a2), a3_(a3), b1_(b1) {}

    double step(double u) override {
        const double y0 = y_curr_;
        const double y1 = y_prev1_;
        const double y2 = y_prev2_;

        y_curr_ = a1_ * y0 + a2_ * y1 + a3_ * y2 + b1_ * u;
        y_prev2_ = y1;
        y_prev1_ = y0;
        return y_curr_;
    }

    void reset() override {
        y_curr_ = 0.0;
        y_prev1_ = 0.0;
        y_prev2_ = 0.0;
    }

    const char* name() const override { return "Model_1_9"; }

    void setInitialState(const std::vector<double>& init) override {
        y_curr_  = init.size() > 0 ? init[0] : 0.0;
        y_prev1_ = init.size() > 1 ? init[1] : 0.0;
        y_prev2_ = init.size() > 2 ? init[2] : 0.0;
    }

private:
    double a1_;
    double a2_;
    double a3_;
    double b1_;
    double y_curr_  = 0.0;
    double y_prev1_ = 0.0;
    double y_prev2_ = 0.0;
};
