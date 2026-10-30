#ifndef MODEL_3_3_H
#define MODEL_3_3_H

#include "Model.h"
#include <string>

// Model 3.3 — стандартный процесс первого порядка:
//   dy/dt = -a*y + b*u
// Решается методом Эйлера с шагом dt:
//   y(t+1) = y(t) + dt*(-a*y(t) + b*u(t))
class Model3_3 : public Model {
public:
    Model3_3(double coeffA, double coeffB, double timeStep);

    double calculateNext(double input) override;
    void reset() override;

    // Разностная схема устойчива при |1 - a*dt| < 1
    bool isStable(std::string& message) const override;

private:
    double a;
    double b;
    double dt;
};

#endif // MODEL_3_3_H
