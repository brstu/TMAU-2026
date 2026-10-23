#include "Model2_5.h"
#include <cmath>

Model2_5::Model2_5(double a_val, double b_val, double y0) : a(a_val), b(b_val), y_current(y0) {}

double Model2_5::step(double u_tau) {
    int sign_u = (u_tau > 0) - (u_tau < 0);
    double nonlinear_term = sign_u * (1.0 - std::exp(-std::abs(u_tau)));
    y_current = a * y_current + b * nonlinear_term;
    return y_current;
}

void Model2_5::reset(double y0) { 
    y_current = y0; 
}