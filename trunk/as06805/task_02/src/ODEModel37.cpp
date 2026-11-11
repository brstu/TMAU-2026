#include "ODEModel37.h"

#include <cmath>
#include <iomanip>
#include <sstream>

ODEModel37::ODEModel37(double a, double b, double dt) : a_(a), b_(b), dt_(dt) {}

double ODEModel37::decay() const {
    return std::exp(a_);
}

double ODEModel37::step(double u) {
    double dy = -std::exp(a_) * y_ + b_ * u;
    y_ += dt_ * dy;
    return y_;
}

void ODEModel37::reset() {
    y_ = 0.0;
}

std::string ODEModel37::name() const {
    return "Model 3.7 (ODE, Euler)";
}

std::vector<std::complex<double>> ODEModel37::poles() const {
    // Дискретный полюс метода Эйлера (см. ODEModel37.h).
    return {std::complex<double>(1.0 - dt_ * std::exp(a_), 0.0)};
}

std::string ODEModel37::equation() const {
    std::ostringstream out;
    out << std::fixed << std::setprecision(4)
        << "dy/dt = -exp(" << a_ << ")*y + " << b_ << "*u, dt = " << dt_;
    return out.str();
}

void ODEModel37::setInitialState(const std::vector<double>& init) {
    y_ = init.empty() ? 0.0 : init.front();
}
