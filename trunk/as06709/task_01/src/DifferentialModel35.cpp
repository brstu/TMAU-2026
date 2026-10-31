#include "DifferentialModel35.h"
#include <cmath>

DifferentialModel35::DifferentialModel35(double b) : b_(b) {}

double DifferentialModel35::calculateNext(double u, double dt) {
    // Explicit Euler method for dy/dt = b*sin(u).
    return y_ + dt * b_ * std::sin(u);
}
