#include "LinearModel17.h"

LinearModel17::LinearModel17(double a, double b1, double b2, double b3)
    : a_(a), b1_(b1), b2_(b2), b3_(b3) {}

double LinearModel17::calculateNext(double u, double) {
    return a_ * y_ + b1_ * u + b2_ * previousU_ + b3_ * previousU2_;
}
