#include "Model2_3.h"
#include <cmath>

Model2_3::Model2_3(double a1_val, double b1_val, double c1_val) 
    : a1(a1_val), b1(b1_val), c1(c1_val) {}

double Model2_3::calculateNext(double u) {
    double current_y = getY();
    double y_next = a1 * current_y - b1 * std::pow(y_prev1, 2) + c1 * u;

    y_prev1 = current_y;
    setY(y_next);

    return y_next;
}

void Model2_3::reset() {
    setY(0.0);
    y_prev1 = 0.0;
}