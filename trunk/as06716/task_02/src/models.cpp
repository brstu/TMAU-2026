#include "models.hpp"

#include <cmath>


LinearModel16::LinearModel16(double a1, double a2, double a3, double b)
    : a1_(a1), a2_(a2), a3_(a3), b_(b) {}

double LinearModel16::step(double u) {
    const double next = a1_ * y0_ + a2_ * y1_ + a3_ * y2_ + b_ * u;
    y2_ = y1_;
    y1_ = y0_;
    y0_ = next;

    return next;
}

double LinearModel16::output() const {
    return y0_;
}

std::string LinearModel16::name() const {
    return "Model 1.6";
}


NonlinearModel210::NonlinearModel210(double a, double b) : a_(a), b_(b) {}

double NonlinearModel210::step(double u) {
    y_ = a_ * std::tanh(y_) + b_ * u * u * u;

    return y_;
}

double NonlinearModel210::output() const {
    return y_;
}

std::string NonlinearModel210::name() const {
    return "Model 2.10";
}


DiffModel34::DiffModel34(double a, double dt, double y0) : a_(a), dt_(dt), y_(y0) {}

double DiffModel34::step(double /*u*/) {
    y_ -= dt_ * a_ * y_ * y_;

    return y_;
}

double DiffModel34::output() const {
    return y_;
}

std::string DiffModel34::name() const {
    return "Model 3.4";
}


double diffModel34Exact(double a, double y0, double t) {
    return y0 / (1.0 + a * y0 * t);
}
