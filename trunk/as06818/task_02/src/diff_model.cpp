#include "diff_model.hpp"

DiffModel36::DiffModel36(double a, double b, double dt, double y_start)
    : a(a), b(b), dt(dt), y_start(y_start), y_curr(y_start) {}

double DiffModel36::step(double u) {
    const double derivative = a * y_curr * y_curr * y_curr + b * u;
    y_curr += derivative * dt;
    return y_curr;
}

void DiffModel36::reset() {
    y_curr = y_start;
}

double DiffModel36::get_y() const {
    return y_curr;
}
