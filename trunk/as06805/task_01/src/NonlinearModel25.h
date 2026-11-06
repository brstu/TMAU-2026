#pragma once

#include "Model.h"
#include <cmath>

class NonlinearModel25 : public Model {
private:
    double a_;
    double b_;
    double yCurr_ = 0.0;

public:
    NonlinearModel25(double a, double b)
        : a_(a), b_(b) {}

    double step(double u) override {
        double signU = 0.0;
        if (u > 0.0) {
            signU = 1.0;
        } else if (u < 0.0) {
            signU = -1.0;
        }

        double nonlin = signU * (1.0 - std::exp(-std::abs(u)));
        yCurr_ = a_ * yCurr_ + b_ * nonlin;
        return yCurr_;
    }

    void reset() override {
        yCurr_ = 0.0;
    }

    std::string name() const override {
        return "Model 2.5 (Nonlinear friction)";
    }
};