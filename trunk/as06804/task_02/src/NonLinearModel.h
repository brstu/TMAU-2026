#ifndef NONLINEARMODEL_H
#define NONLINEARMODEL_H

#include "Model.h"

// Variant 4, model 2.6: y[k+1] = a1*y[k]^3 - a2*y[k-1] + b*u[k]^2.
class NonLinearModel : public Model {
private:
    double a1;
    double a2;
    double b;
    double y_prev = 0.0;
    bool first_step = true;

public:
    NonLinearModel(double a1, double a2, double b, double start_y, double dt);
    void step(double u) override;
};

#endif // NONLINEARMODEL_H
