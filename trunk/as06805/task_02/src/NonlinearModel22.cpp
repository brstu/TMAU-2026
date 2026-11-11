#include "NonlinearModel22.h"

#include <iomanip>
#include <sstream>

NonlinearModel22::NonlinearModel22(double a, double b, double uMin, double uMax)
    : a_(a), b_(b), uMin_(uMin), uMax_(uMax) {}

double NonlinearModel22::saturate(double u) const {
    if (u > uMax_) {
        return uMax_;
    }
    if (u < uMin_) {
        return uMin_;
    }
    return u;
}

double NonlinearModel22::step(double u) {
    y_ = a_ * y_ + b_ * saturate(u);
    return y_;
}

void NonlinearModel22::reset() {
    y_ = 0.0;
}

std::string NonlinearModel22::name() const {
    return "Model 2.2 (Actuator saturation)";
}

std::vector<std::complex<double>> NonlinearModel22::poles() const {
    return {std::complex<double>(a_, 0.0)};
}

std::string NonlinearModel22::equation() const {
    std::ostringstream out;
    out << std::fixed << std::setprecision(4)
        << "y(tau+1) = " << a_ << "*y(tau) + " << b_ << "*sat(u(tau)), "
        << "Umin = " << uMin_ << ", Umax = " << uMax_;
    return out.str();
}

void NonlinearModel22::setInitialState(const std::vector<double>& init) {
    y_ = init.empty() ? 0.0 : init.front();
}
