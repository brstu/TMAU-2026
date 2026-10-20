#pragma once
#include "Model.h"
#include <cmath>

class ODE37 final : public Model {
public:
    ODE37(double a, double b, double dt)
        : a_(a), b_(b), dt_(dt) {
        reset();
    }

    void reset() override {
        y_ = 0.0;
    }

    double step(double u) override {
        const double dy = -std::exp(a_) * y_ + b_ * u;
        y_ = y_ + dt_ * dy;
        return y_;
    }

    const char* name() const override {
        return "Model_3_7";
    }

private:
    double a_;
    double b_;
    double dt_;
    double y_ = 0.0;
};
