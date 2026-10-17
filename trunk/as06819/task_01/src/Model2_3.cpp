#include "Model2_3.h"
#include <cmath>

Model2_3::Model2_3(double a1, double b1, double c1) : a1(a1), b1(b1), c1(c1) {}

double Model2_3::calculateNext(double u) {
    double y_next = a1 * y_curr - b1 * std::pow(y_prev1, 2) + c1 * u;
    y_prev1 = y_curr;
    y_curr = y_next;
    return y_curr;
}

void Model2_3::reset() {
    y_curr = y_prev1 = 0.0;
}
