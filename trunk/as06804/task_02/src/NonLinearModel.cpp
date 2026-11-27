#include "NonLinearModel.h"
#include <cmath>

NonLinearModel::NonLinearModel(double a1, double a2, double b, double start_y, double dt)
    : Model(start_y, dt), a1(a1), a2(a2), b(b) {}

void NonLinearModel::step(double u) {
    const double current_y = getY();

    if (first_step) {
        // Initial previous state is the initial state itself (as in lab #1).
        y_prev = current_y;
        first_step = false;
    }

    const double y_next = a1 * std::pow(current_y, 3) - a2 * y_prev + b * std::pow(u, 2);
    setY(y_next);
    y_prev = current_y;
}
