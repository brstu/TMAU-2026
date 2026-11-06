#ifndef LINEARMODEL_H
#define LINEARMODEL_H

#include "Model.h"

// Model 1.4: y(t+1) = a*y(t) + b*(u(t) - u(t-1))
class LinearModel : public Model {
private:
    double a, b;
    double u_prev;
    bool first_step;
public:
    LinearModel(double a, double b, double start_y, double dt);
    void step(double u) override;
};

#endif