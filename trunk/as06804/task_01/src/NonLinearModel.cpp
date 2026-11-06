#include "NonLinearModel.h"
#include <cmath>

NonLinearModel::NonLinearModel(double a1, double a2, double b, double start_y, double dt)
    : Model(start_y, dt), a1(a1), a2(a2), b(b), y_prev(start_y), first_step(true) {}

void NonLinearModel::step(double u) {
    if (first_step) {
        y_prev = y;
        first_step = false;
    }
    // Формула: y(t+1) = a1*y^3 - a2*y(t-1) + b*u^2
    double y_next = a1 * std::pow(y, 3) - a2 * y_prev + b * std::pow(u, 2);
    y_prev = y;
    y = y_next;
}