#ifndef MODEL_3_7_H
#define MODEL_3_7_H

#include "Model.h"
#include <cmath>

class Model3_7 : public Model {
public:
    Model3_7(double coeffA, double coeffB, double timeStep)
        : a(coeffA), b(coeffB), dt(timeStep) {}

    double calculateNext(double input) override {
        double current_y = getY();
        double next_y = current_y + dt * (-std::exp(a) * current_y + b * input);
        setY(next_y);
        return next_y;
    }

    void reset() override {
        setY(0.0);
    }

private:
   double a;
   double b;
   double dt;
};

#endif // MODEL_3_7_H