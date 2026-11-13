#ifndef MODEL17_H
#define MODEL17_H

#include "Model.h"

class Model17 : public Model {
private:
    double a;
    double b1;
    double b2;
    double b3;

public:
    Model17(double a, double b1, double b2, double b3);

    double calculateNext(
        double y,
        double yPrev,
        double u,
        double uPrev,
        double uPrev2
    ) override;
};

#endif