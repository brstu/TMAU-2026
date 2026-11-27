#ifndef DIFFEQMODEL_H
#define DIFFEQMODEL_H

#include "Model.h"

class DiffEqModel : public Model {
private:
    double a1;
    double a2;
    double b;

public:
    DiffEqModel(double a1, double a2, double b, double start_y, double dt);
    void step(double u) override;
};

#endif
