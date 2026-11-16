#include "linear_model.hpp"

LinearModel18::LinearModel18(double a1, double a2, double b1, double b2,
                             double y_start)
    : a1(a1), a2(a2), b1(b1), b2(b2), y_start(y_start),
      y_curr(y_start), y_prev(0.0), u_prev(0.0) {}

double LinearModel18::step(double u) {
    const double y_next =
        a1 * y_curr + a2 * y_prev + b1 * u + b2 * u_prev;

    y_prev = y_curr;
    y_curr = y_next;
    u_prev = u;

    return y_next;
}

void LinearModel18::reset() {
    y_curr = y_start;
    y_prev = 0.0;
    u_prev = 0.0;
}

double LinearModel18::get_a1() const {
    return a1;
}

double LinearModel18::get_a2() const {
    return a2;
}

double LinearModel18::get_y() const {
    return y_curr;
}
