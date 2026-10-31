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
    y_ = 0.0;
    previousY_ = 0.0;
    previousU_ = 0.0;
    previousU2_ = 0.0;
}

double Model::currentY() const {
    return y_;
}

double Model::previousY() const {
    return previousY_;
}

double Model::previousU() const {
    return previousU_;
}

double Model::previousU2() const {
    return previousU2_;
}
