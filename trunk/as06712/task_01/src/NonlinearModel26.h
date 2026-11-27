#pragma once
#include "Model.h"

class NonlinearModel26 : public Model {
public:
    NonlinearModel26(double a1, double a2, double b)
        : a1_(a1), a2_(a2), b_(b) {
        reset();
    }

    void reset() override {
        y_ = 0.0;
        yPrev_ = 0.0;
    }

    double step(double u) override {
        double y_next = a1_ * y_ * y_ * y_   // a1 * y^3
                      - a2_ * yPrev_          // -a2 * y_{tau-1}
                      + b_ * u * u;           // +b * u^2
        yPrev_ = y_;
        y_ = y_next;
        return y_;
    }

    const char* name() const override { return "Model 2.6 (Nonlinear polynomial)"; }

private:
    double a1_, a2_, b_;
    double y_ = 0.0;
    double yPrev_ = 0.0;
};
