#ifndef MODEL_2_9_H
#define MODEL_2_9_H

#include "Model.h"
#include <string>

// Model 2.9 — модуляция управляющего воздействия квадратным корнем:
//   y(t+1) = a*y(t) + b*sqrt(|u(t)|)*sign(u(t))
class Model2_9 : public Model {
public:
    Model2_9(double coeffA, double coeffB);

    double calculateNext(double input) override;
    void reset() override;

    // Однородная часть y(t+1) = a*y(t) устойчива при |a| < 1
    bool isStable(std::string& message) const override;

private:
    static double sign(double value);

    double a;
    double b;
};

#endif // MODEL_2_9_H
