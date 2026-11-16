#include "nonlinear_model.hpp"

#include <algorithm>

NonlinearModel22::NonlinearModel22(double a, double b,
                                   double u_min, double u_max,
                                   double y_start)
    : a(a), b(b), u_min(u_min), u_max(u_max), y_start(y_start),
      y_curr(y_start) {}

double NonlinearModel22::sat(double u) const {
    return std::clamp(u, u_min, u_max);
}

double NonlinearModel22::step(double u) {
    y_curr = a * y_curr + b * sat(u);
    return y_curr;
}

void NonlinearModel22::reset() {
    y_curr = y_start;
}

double NonlinearModel22::get_y() const {
    return y_curr;
}
