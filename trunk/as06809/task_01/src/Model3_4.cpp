#include "Model3_4.h"

Model3_4::Model3_4(double p_a, double p_dt) : a(p_a), dt(p_dt) {}

double Model3_4::calculateNext(double u) {
    double current_y = getY();
    double next_y = current_y + dt * (-a * current_y * current_y);
    setY(next_y);
    return next_y;
}

void Model3_4::reset() {
    setY(0.0);
}