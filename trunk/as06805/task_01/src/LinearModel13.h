#pragma once

#include "Model.h"

class LinearModel13 : public Model {
private:
    double a1_;
    double a2_;
    double b_;
    double yCurr_ = 0.0;
    double yPrev_ = 0.0;

public:
    LinearModel13(double a1, double a2, double b)
        : a1_(a1), a2_(a2), b_(b) {}

    double step(double u) override {
        double yNext = a1_ * yCurr_ + a2_ * yPrev_ + b_ * u;
        yPrev_ = yCurr_;
        yCurr_ = yNext;
        return yCurr_;
    }

    void reset() override {
        yCurr_ = 0.0;
        yPrev_ = 0.0;
    }

    std::string name() const override {
        return "Model 1.3 (Linear 2nd order)";
    }
};