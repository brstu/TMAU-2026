#include "DiffEqModel.h"
#include <cmath>

DiffEqModel::DiffEqModel(double a1, double a2, double b, double start_y, double dt)
    : Model(start_y, dt), a1(a1), a2(a2), b(b) {}

void DiffEqModel::step(double u) {
    // dy/dt = -a1*y - a2*y^2 + b*u
    double dydt = -a1 * y - a2 * std::pow(y, 2) + b * u;
    y = y + dt * dydt; // Метод Эйлера
}