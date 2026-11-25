#pragma once
#include "Model.h"
#include <cmath>

class NonlinearModel25 final : public Model {
public:
    NonlinearModel25(double a, double b)
        : a_(a), b_(b) {
        reset();
    }

    void reset() override {
        y_ = 0.0;
    }

    double step(double u) override {
        double sign_u = 0.0;
        if (u > 0.0) {
            sign_u = 1.0;
        } else if (u < 0.0) {
            sign_u = -1.0;
        }
        
        const double term = b_ * sign_u * (1.0 - std::exp(-std::abs(u)));
        y_ = a_ * y_ + term;
        return y_;
    }

    const char* name() const override {
        return "Model_2_5";
    }

private:
    double a_;
    double b_;
    double y_ = 0.0;
};
