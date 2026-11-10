#include "models.h"

// Realization LinearModel1_4
LinearModel1_4::LinearModel1_4(double a_val, double b_val)
    : a(a_val), b(b_val) {}

double LinearModel1_4::calculateNext(double u) {
    y_curr = a * y_curr + b * (u - u_prev);
    u_prev = u;
    return y_curr;
}

void LinearModel1_4::reset() {
    y_curr = 0.0;
    u_prev = 0.0;
}

// Realization NonlinearModel2_8
NonlinearModel2_8::NonlinearModel2_8(double a_val, double b_val, double c_val)
    : a(a_val), b(b_val), c(c_val) {}

double NonlinearModel2_8::calculateNext(double u) {
    double y_next = a * y_curr - b * std::pow(y_prev, 2) + c * u + std::sin(u);
    y_prev = y_curr;
    y_curr = y_next;
    return y_curr;
}

void NonlinearModel2_8::reset() {
    y_curr = 0.0;
    y_prev = 0.0;
}

// Realization DiffModel3_2
DiffModel3_2::DiffModel3_2(double b_val, double dt_val)
    : b(b_val), dt(dt_val) {}

double DiffModel3_2::calculateNext(double u) {
    y_curr += dt * b * u;
    return y_curr;
}

void DiffModel3_2::reset() {
    y_curr = 0.0;
}