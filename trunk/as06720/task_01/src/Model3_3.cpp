#include "Model3_3.h"
#include <cmath>
#include <sstream>

Model3_3::Model3_3(double coeffA, double coeffB, double timeStep)
    : a(coeffA), b(coeffB), dt(timeStep) {}

double Model3_3::calculateNext(double input) {
    const double y_curr = output();

    // Один шаг явного метода Эйлера
    const double y_next = y_curr + dt * (-a * y_curr + b * input);

    storeOutput(y_next);
    return y_next;
}

void Model3_3::reset() {
    storeOutput(0.0);
}

bool Model3_3::isStable(std::string& message) const {
    const double z = 1.0 - a * dt;
    std::ostringstream out;
    out.precision(4);
    out << std::fixed << "Euler scheme root z = 1 - a*dt = " << z
        << ", |z| = " << std::fabs(z) << " (must be < 1)";
    message = out.str();
    return std::fabs(z) < 1.0;
}
