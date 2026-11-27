#pragma once
#include "Model.h"
#include <deque>

class LinearModel12 : public Model {
public:
    LinearModel12(double a, double b, int k)
        : a_(a), b_(b), k_(k) {
        reset();
    }

    void reset() override {
        y_ = 0.0;
        uHistory_.assign(k_ + 1, 0.0);
    }

    double step(double u) override {
        uHistory_.push_back(u);
        if ((int)uHistory_.size() > k_ + 1)
            uHistory_.pop_front();

        // u_{tau-k}
        double u_delayed = uHistory_.front();

        double y_next = a_ * y_ + b_ * u_delayed;
        y_ = y_next;
        return y_;
    }

    const char* name() const override { return "Model 1.2 (Linear with delay)"; }

private:
    double a_, b_;
    int k_;
    double y_ = 0.0;
    std::deque<double> uHistory_;
};
