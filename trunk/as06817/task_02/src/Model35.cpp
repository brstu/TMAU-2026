#include "Model35.h"

#include <cmath>

Model35::Model35(double b, double dt, double initialY)
    : b_(b),
      dt_(dt),
      initialY_(initialY) {
    reset();
}

double Model35::step(double u) {
    y_ += dt_ * b_ * std::sin(u);
    return y_;
}

void Model35::reset() {
    y_ = initialY_;
}

std::string Model35::name() const {
    return "Model 3.5";
}

bool Model35::isLinear() const {
    // Правая часть содержит sin(u), поэтому модель нелинейна по входу.
    return false;
}

std::vector<double> Model35::characteristicPolynomial() const {
    return {};
}
