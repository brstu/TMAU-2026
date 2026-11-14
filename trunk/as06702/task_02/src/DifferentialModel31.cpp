#include "DifferentialModel31.h"

#include <stdexcept>

DifferentialModel31::DifferentialModel31(double a, double initialY)
    : Model(initialY), a_(a) {}

double DifferentialModel31::step(double /*u*/, double dt) {
    if (dt <= 0.0) {
        throw std::invalid_argument("The integration step dt must be positive");
    }
    const double derivative = -a_ * y_;
    y_ += dt * derivative;
    return y_;
}

std::string DifferentialModel31::name() const {
    return "Model 3.1 - Differential";
}

double DifferentialModel31::a() const { return a_; }
