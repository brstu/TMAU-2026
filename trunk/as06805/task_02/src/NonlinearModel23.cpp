#include "NonlinearModel23.h"

#include <iomanip>
#include <sstream>

NonlinearModel23::NonlinearModel23(double a, double b, double delta)
    : a_(a), b_(b), delta_(delta) {}

double NonlinearModel23::deadZone(double u) const {
    if (u > delta_) {
        return u - delta_;
    }
    if (u < -delta_) {
        return u + delta_;
    }
    return 0.0;
}

double NonlinearModel23::step(double u) {
    y_ = a_ * y_ + b_ * deadZone(u);
    return y_;
}

void NonlinearModel23::reset() {
    y_ = 0.0;
}

std::string NonlinearModel23::name() const {
    return "Model 2.3 (Dead-zone non-linearity)";
}

std::vector<std::complex<double>> NonlinearModel23::poles() const {
    return {std::complex<double>(a_, 0.0)};
}

std::string NonlinearModel23::equation() const {
    std::ostringstream out;
    out << std::fixed << std::setprecision(4)
        << "y(tau+1) = " << a_ << "*y(tau) + " << b_ << "*dz(u(tau)), "
        << "delta = " << delta_;
    return out.str();
}

void NonlinearModel23::setInitialState(const std::vector<double>& init) {
    y_ = init.empty() ? 0.0 : init.front();
}
