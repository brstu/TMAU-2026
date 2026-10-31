#include "NonlinearModel21.h"
#include <cmath>

NonlinearModel21::NonlinearModel21(double a, double b, double c, double d)
    : a_(a), b_(b), c_(c), d_(d) {
}

double NonlinearModel21::calculateNext(double u, double) {
    return a_ * currentY()
        - b_ * previousY() * previousY()
        + c_ * u
        + d_ * std::sin(previousU());
}