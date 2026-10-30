#pragma once
#include "Model.h"
#include "StabilityAnalyzer.h"
#include <iostream>

class LinearModel13 final : public Model {
public:
    LinearModel13(double a1, double a2, double b)
        : a1_(a1), a2_(a2), b_(b) {
        if (!StabilityAnalyzer::isStableLinear13(a1_, a2_)) {
            std::cerr << "\n[WARNING] Model 1.3 is UNSTABLE! Roots outside Z-plane.\n\n";
        }
        reset();
    }

    void reset() override {
        y_curr_ = 0.0;
        y_prev_ = 0.0;
    }

    double step(double u) override {
        const double y_next = a1_ * y_curr_ + a2_ * y_prev_ + b_ * u;
        y_prev_ = y_curr_;
        y_curr_ = y_next;
        return y_curr_;
    }

    const char* name() const override {
        return "Model_1_3";
    }

private:
    double a1_;
    double a2_;
    double b_;
    double y_curr_ = 0.0;
    double y_prev_ = 0.0;
};