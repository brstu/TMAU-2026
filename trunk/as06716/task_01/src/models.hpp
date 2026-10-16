#pragma once

#include <cmath>


class Model {
public:
    virtual ~Model() = default;

    virtual double step(double u) = 0;

    virtual double output() const = 0;
};


// model 1.6
class LinearModel16 : public Model {
public:
    LinearModel16(double a1, double a2, double a3, double b): a1_(a1), a2_(a2), a3_(a3), b_(b) {}

    double step(double u) override {
        const double next = a1_ * y0_ + a2_ * y1_ + a3_ * y2_ + b_ * u;
        y2_ = y1_;
        y1_ = y0_;
        y0_ = next;

        return next;
    }

    double output() const override { return y0_; }

private:
    double a1_;
    double a2_;
    double a3_;
    double b_;
    double y0_ = 0.0;   // y[t]
    double y1_ = 0.0;   // y[t-1]
    double y2_ = 0.0;   // y[t-2]
};


// model 2.10
class NonlinearModel210 : public Model {
public:
    NonlinearModel210(double a, double b) : a_(a), b_(b) {}

    double step(double u) override {
        y_ = a_ * std::tanh(y_) + b_ * u * u * u;

        return y_;
    }

    double output() const override { return y_; }

private:
    double a_;
    double b_;
    double y_ = 0.0;
};


// model 3.4
class DiffModel34 : public Model {
public:
    DiffModel34(double a, double dt, double y0) : a_(a), dt_(dt), y_(y0) {}

    double step(double /*u*/) override {
        y_ -= dt_ * a_ * y_ * y_;

        return y_;
    }

    double output() const override { return y_; }

private:
    double a_;
    double dt_;
    double y_;
};
