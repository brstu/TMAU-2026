#include "Model.h"

double Model::step(double u, double dt) {
    const double next = calculateNext(u, dt);
    previousY_ = y_;
    y_ = next;
    previousU2_ = previousU_;
    previousU_ = u;
    return y_;
}

void Model::reset() {
    y_ = previousY_ = previousU_ = previousU2_ = 0.0;
}
