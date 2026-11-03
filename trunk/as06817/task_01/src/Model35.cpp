#include "Model35.h"
#include <cmath>

Model35::Model35(double b, double dt)
    : b(b), dt(dt) {
}

double Model35::calculateNext(
    double y,
    double yPrev,
    double u,
    double uPrev,
    double uPrev2
) {
    return y + dt * b * std::sin(u);
}