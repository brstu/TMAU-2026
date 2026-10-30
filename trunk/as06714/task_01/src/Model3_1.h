#ifndef MODEL_3_1_H
#define MODEL_3_1_H

#include "Model.h"
#include <string>

// Model 3.1 — чистое линейное затухание:
//   dy/dt = -a*y
// Решается методом Эйлера с шагом dt:
//   y(t+1) = y(t) - dt*a*y(t)
// Вход u в уравнение не входит, поэтому процесс определяется
// начальным условием y0 (при y0 = 0 выход тождественно равен нулю).
class Model3_1 : public Model {
public:
    Model3_1(double coeffA, double timeStep, double initialY);

    double calculateNext(double input) override;
    void reset() override;

    // Разностная схема устойчива при |1 - a*dt| < 1
    bool isStable(std::string& message) const override;

private:
    double a;
    double dt;
    double y0;
};

#endif // MODEL_3_1_H
