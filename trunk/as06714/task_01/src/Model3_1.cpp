#include "Model3_1.h"
#include <cmath>
#include <sstream>

Model3_1::Model3_1(double coeffA, double timeStep, double initialY)
    : a(coeffA), dt(timeStep), y0(initialY) {
    reset();
}

double Model3_1::calculateNext(double /*input*/) {
    const double y_curr = getY();

    // Метод Эйлера: y(t+1) = y(t) - dt * a * y(t)
    const double y_next = y_curr - dt * a * y_curr;

    setY(y_next);
    return y_next;
}

void Model3_1::reset() {
    setY(y0);
}

bool Model3_1::isStable(std::string& message) const {
    const double z = 1.0 - a * dt;
    std::ostringstream out;
    out.precision(4);
    out << std::fixed << "Euler scheme root z = 1 - a*dt = " << z
        << ", |z| = " << std::fabs(z) << " (must be < 1)";
    message = out.str();
    return std::fabs(z) < 1.0;
}
