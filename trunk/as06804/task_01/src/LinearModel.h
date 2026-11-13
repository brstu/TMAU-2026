#ifndef LINEARMODEL_H
#define LINEARMODEL_H

#include "Model.h"

class LinearModel : public Model {
private:
    double a;
    double b;
    double u_prev = 0.0;
    bool first_step = true;

public:
    LinearModel(double a, double b, double start_y, double dt);
    void step(double u) override;
};

#endif
