#pragma once
#include "IModel.hpp"
#include <cmath>

// Model 1.6:  y[k+1] = a1*y[k] + a2*y[k-1] + a3*y[k-2] + b*u[k]
class ThirdOrderLinear final : public IModel {
public:
    ThirdOrderLinear(double a1, double a2, double a3, double b)
        : a1_(a1), a2_(a2), a3_(a3), b_(b) {
        reset();
    }

    void reset() override { y0_ = y1_ = y2_ = 0.0; }

    double advance(double u) override {
        const double nxt = a1_ * y0_ + a2_ * y1_ + a3_ * y2_ + b_ * u;
        y2_ = y1_;
        y1_ = y0_;
        y0_ = nxt;
        return y0_;
    }

    const char* title() const override {
        return "Model 1.6 (third-order linear)";
    }

private:
    double a1_, a2_, a3_, b_;
    double y0_ = 0.0, y1_ = 0.0, y2_ = 0.0;
};

// Model 2.8:  y[k+1] = a*y[k]*(1-y[k]) + b*u[k] + c*sin(y[k-1]*u[k])
class LogisticMap final : public IModel {
public:
    LogisticMap(double a, double b, double c, double y0)
        : a_(a), b_(b), c_(c), y0_(y0) {
        reset();
    }

    void reset() override { cur_ = y0_; prev_ = 0.0; }

    double advance(double u) override {
        const double nxt =
            a_ * cur_ * (1.0 - cur_)
            + b_ * u
            + c_ * std::sin(prev_ * u);
        prev_ = cur_;
        cur_ = nxt;
        return cur_;
    }

    const char* title() const override {
        return "Model 2.8 (logistic map with disturbance)";
    }

private:
    double a_, b_, c_, y0_;
    double cur_ = 0.0, prev_ = 0.0;
};

// Model 3.10:  dy/dt = -a*y + b + u 
class OffsetODE final : public IModel {
public:
    OffsetODE(double a, double b, double dt, double y0)
        : a_(a), b_(b), dt_(dt), y0_(y0) {
        reset();
    }

    void reset() override { y_ = y0_; }

    double advance(double u) override {
        y_ += dt_ * (-a_ * y_ + b_ + u);
        return y_;
    }

    const char* title() const override {
        return "Model 3.10 (offset ODE, Euler)";
    }

private:
    double a_, b_, dt_, y0_;
    double y_ = 0.0;
};