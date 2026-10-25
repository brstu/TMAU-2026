#include "Model3_7.h"
#include <cmath>

Model3_7::Model3_7(double a_val, double b_val, double dt_val) 
    : a(a_val), b(b_val), dt(dt_val) {}

double Model3_7::calculateNext(double u) {
    double current_y = getY();
    double y_next = current_y + dt * (-std::exp(a) * current_y + b * u);

    setY(y_next);

    return y_next;
}

void Model3_7::reset() {
    setY(0.0);
}