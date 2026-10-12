#include "Model21.h"
#include <cmath>

Model21::Model21(double a, double b, double c, double d)
    : a(a), b(b), c(c), d(d) {
}

double Model21::calculateNext(
    double y,
    double yPrev,
    double u,
    double uPrev,
    double uPrev2
) {
    return a * y
        - b * yPrev * yPrev
        + c * u
        + d * std::sin(uPrev);
}