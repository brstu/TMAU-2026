#include "LinearModel13.h"

LinearModel13::LinearModel13(double a1, double a2, double b,
                             double initialY, double initialPreviousY)
    : Model(initialY), a1_(a1), a2_(a2), b_(b),
      previousY_(initialPreviousY), initialPreviousY_(initialPreviousY) {}

double LinearModel13::step(double u, double /*dt*/) {
    const double nextY = a1_ * y_ + a2_ * previousY_ + b_ * u;
    previousY_ = y_;
    y_ = nextY;
    return y_;
}

std::string LinearModel13::name() const {
    return "Model 1.3 - Linear";
}

void LinearModel13::reset() {
    Model::reset();
    previousY_ = initialPreviousY_;
}

double LinearModel13::a1() const { return a1_; }
double LinearModel13::a2() const { return a2_; }
double LinearModel13::b() const { return b_; }
double LinearModel13::previousValue() const { return previousY_; }
