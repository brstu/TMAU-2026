#include "Model17.h"

Model17::Model17(double a, double b1, double b2, double b3, double initialY)
    : a_(a),
      b1_(b1),
      b2_(b2),
      b3_(b3),
      initialY_(initialY) {
    reset();
}

double Model17::step(double u) {
    const double next = a_ * y_
        + b1_ * u
        + b2_ * uPrev1_
        + b3_ * uPrev2_;

    uPrev2_ = uPrev1_;
    uPrev1_ = u;
    y_ = next;

    return y_;
}

void Model17::reset() {
    y_ = initialY_;
    uPrev1_ = 0.0;
    uPrev2_ = 0.0;
}

std::string Model17::name() const {
    return "Model 1.7";
}

bool Model17::isLinear() const {
    return true;
}

std::vector<double> Model17::characteristicPolynomial() const {
    // Характеристическое уравнение: z - a = 0.
    return {1.0, -a_};
}
