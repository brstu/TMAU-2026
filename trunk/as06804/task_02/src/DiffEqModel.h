#ifndef DIFFEQMODEL_H
#define DIFFEQMODEL_H

#include "Model.h"

// Variant 4, model 3.8: dy/dt = -a1*y - a2*y^2 + b*u (explicit Euler).
class DiffEqModel : public Model {
private:
    double a1;
    double a2;
    double b;

public:
    DiffEqModel(double a1, double a2, double b, double start_y, double dt);
    void step(double u) override;
};

#endif // DIFFEQMODEL_H
