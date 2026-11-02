#include "Model3_7.h"
#include <cmath>

Model3_7::Model3_7(double coeffA, double coeffB, double timeStep)
    : a(coeffA), b(coeffB), dt(timeStep) {}

double Model3_7::calculateNext(double input) {
    // ИСПРАВЛЕНО: переменная переименована
    double current_y = getY();

    // Метод Эйлера: y_{t+1} = y_t + dt * (-e^a * y_t + b * u)
    double next_y = current_y + dt * (-std::exp(a) * current_y + b * input);

    setY(next_y);
    return next_y;
}

void Model3_7::reset() {
    setY(0.0);
}