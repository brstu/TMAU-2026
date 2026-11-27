#include "DiffEqModel.h"
#include <cmath>

DiffEqModel::DiffEqModel(double a1, double a2, double b, double start_y, double dt)
    : Model(start_y, dt), a1(a1), a2(a2), b(b) {}

void DiffEqModel::step(double u) {
    const double current_y = getY();
    const double dydt = -a1 * current_y - a2 * std::pow(current_y, 2) + b * u;
    setY(current_y + getDt() * dydt);
}
