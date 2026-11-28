#pragma once
#include "Model.h"

class ODE310 : public Model {
public:
    ODE310(double a, double b, double dt)
        : a_(a), b_(b), dt_(dt) {
        reset();
    }

    void reset() override { y_ = 0.0; }

    double step(double u) override {
        double dy = -a_ * y_ + b_ + u;
        y_ = y_ + dt_ * dy;
        return y_;
    }

    const char* name() const override { return "Model 3.10 (ODE, Euler)"; }

private:
    double a_, b_, dt_;
    double y_ = 0.0;
};
