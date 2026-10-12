#include "Model17.h"

Model17::Model17(double a, double b1, double b2, double b3)
    : a(a), b1(b1), b2(b2), b3(b3) {
}

double Model17::calculateNext(
    double y,
    double yPrev,
    double u,
    double uPrev,
    double uPrev2
) {
    return a * y
        + b1 * u
        + b2 * uPrev
        + b3 * uPrev2;
}