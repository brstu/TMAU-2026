#include "LinearModel.h"

LinearModel::LinearModel(double a, double b, double start_y, double dt)
    : Model(start_y, dt), a(a), b(b) {}

void LinearModel::step(double u) {
    if (first_step) {
        // At the very first sample there is no previous input; the model treats
        // u[-1] as equal to u[0], exactly as in laboratory work #1.
        u_prev = u;
        first_step = false;
    }

    const double y_next = a * getY() + b * (u - u_prev);
    setY(y_next);
    u_prev = u;
}
