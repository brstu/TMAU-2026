#pragma once

#include "Model.h"
#include <cmath>

class ODEModel37 : public Model {
private:
    double a_;
    double b_;
    double dt_;
    double yCurr_ = 0.0;

public:
    ODEModel37(double a, double b, double dt)
        : a_(a), b_(b), dt_(dt) {}

    double step(double u) override {
        double dy = -std::exp(a_) * yCurr_ + b_ * u;
        yCurr_ += dt_ * dy;
        return yCurr_;
    }

    void reset() override {
        yCurr_ = 0.0;
    }

    std::string name() const override {
        return "Model 3.7 (ODE, Euler)";
    }
};