#include "NonlinearModel25.h"

#include <cmath>
#include <iomanip>
#include <sstream>

NonlinearModel25::NonlinearModel25(double a, double b) : a_(a), b_(b) {}

double NonlinearModel25::friction(double u) const {
    double signU = 0.0;
    if (u > 0.0) {
        signU = 1.0;
    } else if (u < 0.0) {
        signU = -1.0;
    }

    return signU * (1.0 - std::exp(-std::abs(u)));
}

double NonlinearModel25::step(double u) {
    y_ = a_ * y_ + b_ * friction(u);
    return y_;
}

void NonlinearModel25::reset() {
    y_ = 0.0;
}

std::string NonlinearModel25::name() const {
    return "Model 2.5 (Nonlinear friction)";
}

std::vector<std::complex<double>> NonlinearModel25::poles() const {
    return {std::complex<double>(a_, 0.0)};
}

std::string NonlinearModel25::equation() const {
    std::ostringstream out;
    out << std::fixed << std::setprecision(4)
        << "y(tau+1) = " << a_ << "*y(tau) + " << b_
        << "*sign(u(tau))*(1 - exp(-|u(tau)|))";
    return out.str();
}

void NonlinearModel25::setInitialState(const std::vector<double>& init) {
    y_ = init.empty() ? 0.0 : init.front();
}
