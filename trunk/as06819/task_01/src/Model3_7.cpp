#include "Model3_7.h"
#include <cmath>

Model3_7::Model3_7(double a, double b, double dt) : a(a), b(b), dt(dt) {}

double Model3_7::calculateNext(double u) {
    double y_next = y_curr + dt * (-std::exp(a) * y_curr + b * u);
    y_curr = y_next;
    return y_curr;
}

void Model3_7::reset() {
    y_curr = 0.0;
}
