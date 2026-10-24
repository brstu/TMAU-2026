#include "Model2_10.h"
#include <cmath>

Model2_10::Model2_10(double p_a, double p_b, double p_c, double p_d) 
    : a(p_a), b(p_b), c(p_c), d(p_d) {}

double Model2_10::calculateNext(double u) {
    double current_y = getY();
    double next_y = a * current_y - b * std::pow(y_prev1, 2) + c * u + d * std::sin(u_prev1);
    y_prev1 = current_y;
    setY(next_y);
    u_prev1 = u;
    return next_y;
}

void Model2_10::reset() {
    setY(0.0);
    y_prev1 = 0.0;
    u_prev1 = 0.0;
}