#include "LinearModel11.h"

#include <iomanip>
#include <sstream>

LinearModel11::LinearModel11(double a, double b) : a_(a), b_(b) {}

double LinearModel11::step(double u) {
    y_ = a_ * y_ + b_ * u;
    return y_;
}

void LinearModel11::reset() {
    y_ = 0.0;
}

std::string LinearModel11::name() const {
    return "Model 1.1 (Linear 1st order)";
}

std::vector<std::complex<double>> LinearModel11::poles() const {
    return {std::complex<double>(a_, 0.0)};
}

std::string LinearModel11::equation() const {
    std::ostringstream out;
    out << std::fixed << std::setprecision(4)
        << "y(tau+1) = " << a_ << "*y(tau) + " << b_ << "*u(tau)";
    return out.str();
}

void LinearModel11::setInitialState(const std::vector<double>& init) {
    y_ = init.empty() ? 0.0 : init.front();
}
