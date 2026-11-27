#ifndef LINEARMODEL_H
#define LINEARMODEL_H

#include "Model.h"

// Variant 4, model 1.4: y[k+1] = a*y[k] + b*(u[k] - u[k-1]).
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

#endif // LINEARMODEL_H
