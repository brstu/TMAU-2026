#include "Model.h"

Model::Model(double initialY) : y_(initialY), initialY_(initialY) {}

void Model::reset() {
    y_ = initialY_;
}

double Model::value() const {
    return y_;
}
