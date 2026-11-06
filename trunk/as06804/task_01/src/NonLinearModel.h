#ifndef NONLINEARMODEL_H
#define NONLINEARMODEL_H

#include "Model.h"

// Model 2.6: y(t+1) = a1*y(t)^3 - a2*y(t-1) + b*u(t)^2
class NonLinearModel : public Model {
private:
    double a1, a2, b;
    double y_prev;
    bool first_step;
public:
    NonLinearModel(double a1, double a2, double b, double start_y, double dt);
    void step(double u) override;
};
#endif
