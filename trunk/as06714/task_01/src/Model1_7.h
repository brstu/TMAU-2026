#ifndef MODEL_1_7_H
#define MODEL_1_7_H

#include "Model.h"
#include <string>

// Model 1.7 — система с многошаговой историей управления:
//   y(t+1) = a*y(t) + b1*u(t) + b2*u(t-1) + b3*u(t-2)
class Model1_7 : public Model {
public:
    Model1_7(double coeffA, double coeffB1, double coeffB2, double coeffB3);

    double calculateNext(double input) override;
    void reset() override;

    // Характеристическое уравнение z - a = 0: устойчивость при |a| < 1
    bool isStable(std::string& message) const override;

private:
    double a;
    double b1;
    double b2;
    double b3;
    double u_prev1 = 0.0;   // u(t-1)
    double u_prev2 = 0.0;   // u(t-2)
};

#endif // MODEL_1_7_H
