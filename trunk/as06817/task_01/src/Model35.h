#ifndef MODEL35_H
#define MODEL35_H

#include "Model.h"

class Model35 : public Model {
private:
    double b;
    double dt;

public:
    Model35(double b, double dt);

    double calculateNext(
        double y,
        double yPrev,
        double u,
        double uPrev,
        double uPrev2
    ) override;
};

#endif