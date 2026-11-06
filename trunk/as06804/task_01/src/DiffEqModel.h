z#ifndef DIFFEQMODEL_H
#define DIFFEQMODEL_H

#include "Model.h"

// Model 3.8: dy/dt = -a1*y - a2*y^2 + b*u
// Euler method: y(t+1) = y(t) + dt * (-a1*y - a2*y^2 + b*u)
class DiffEqModel : public Model {
private:
    double a1, a2, b;
public:
    DiffEqModel(double a1, double a2, double b, double start_y, double dt);
    void step(double u) override;
};

#endif