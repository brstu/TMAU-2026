#include "LinearModel.h"

LinearModel::LinearModel(double a, double b, double start_y, double dt) 
    : Model(start_y, dt), a(a), b(b), u_prev(0), first_step(true) {}

void LinearModel::step(double u) {
    if (first_step) {
        u_prev = u;
        first_step = false;
    }
    // Формула: y(t+1) = a*y(t) + b*(u(t) - u(t-1))
    double y_next = a * y + b * (u - u_prev);
    y = y_next;
    u_prev = u;
}