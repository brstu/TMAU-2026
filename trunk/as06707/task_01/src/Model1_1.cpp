#include "Model1_1.h"
#include <cmath>

Model1_1::Model1_1(double a_val, double b_val, double y0) : a(a_val), b(b_val), y_current(y0) {}

double Model1_1::step(double u_tau) {
    y_current = a * y_current + b * u_tau;
    return y_current;
}

void Model1_1::reset(double y0) { 
    y_current = y0; 
}