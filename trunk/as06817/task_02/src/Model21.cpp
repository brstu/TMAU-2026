#include "Model21.h"

#include <cmath>

Model21::Model21(double a, double b, double c, double d, double initialY)
    : a_(a),
      b_(b),
      c_(c),
      d_(d),
      initialY_(initialY) {
    reset();
}

double Model21::step(double u) {
    const double next = a_ * y_
        - b_ * yPrev_ * yPrev_
        + c_ * u
        + d_ * std::sin(uPrev_);

    yPrev_ = y_;
    y_ = next;
    uPrev_ = u;

    return y_;
}

void Model21::reset() {
    y_ = initialY_;
    yPrev_ = initialY_;
    uPrev_ = 0.0;
}

std::string Model21::name() const {
    return "Model 2.1";
}

bool Model21::isLinear() const {
    return false;
}

std::vector<double> Model21::characteristicPolynomial() const {
    return {};
}
