#ifndef MODEL21_H
#define MODEL21_H

#include "Model.h"

class Model21 : public Model {
private:
    double a;
    double b;
    double c;
    double d;

public:
    Model21(double a, double b, double c, double d);

    double calculateNext(
        double y,
        double yPrev,
        double u,
        double uPrev,
        double uPrev2
    ) override;
};

#endif