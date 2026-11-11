#include "LinearModel13.h"

#include <cmath>
#include <iomanip>
#include <sstream>

LinearModel13::LinearModel13(double a1, double a2, double b)
    : a1_(a1), a2_(a2), b_(b) {}

double LinearModel13::step(double u) {
    double yNext = a1_ * yCurr_ + a2_ * yPrev_ + b_ * u;
    yPrev_ = yCurr_;
    yCurr_ = yNext;
    return yCurr_;
}

void LinearModel13::reset() {
    yCurr_ = 0.0;
    yPrev_ = 0.0;
}

std::string LinearModel13::name() const {
    return "Model 1.3 (Linear 2nd order)";
}

std::vector<std::complex<double>> LinearModel13::poles() const {
    // z^2 - a1*z - a2 = 0  =>  z = (a1 +/- sqrt(a1^2 + 4*a2)) / 2
    std::complex<double> disc = std::complex<double>(a1_ * a1_ + 4.0 * a2_, 0.0);
    std::complex<double> sq = std::sqrt(disc);
    std::complex<double> half(0.5, 0.0);

    return {half * (std::complex<double>(a1_, 0.0) + sq),
            half * (std::complex<double>(a1_, 0.0) - sq)};
}

std::string LinearModel13::equation() const {
    std::ostringstream out;
    out << std::fixed << std::setprecision(4)
        << "y(tau+1) = " << a1_ << "*y(tau) + " << a2_ << "*y(tau-1) + "
        << b_ << "*u(tau)";
    return out.str();
}

void LinearModel13::setInitialState(const std::vector<double>& init) {
    yCurr_ = init.size() > 0 ? init[0] : 0.0;
    yPrev_ = init.size() > 1 ? init[1] : 0.0;
}
