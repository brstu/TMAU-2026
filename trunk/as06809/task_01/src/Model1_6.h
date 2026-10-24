#pragma once
#include "Model.h"

class Model1_6 : public Model {
private:
    // SonarCloud требует каждую переменную на новой строке
    double a1;
    double a2;
    double a3;
    double b;
    double y_prev1 = 0.0;
    double y_prev2 = 0.0;
public:
    Model1_6(double p_a1, double p_a2, double p_a3, double p_b);
    double calculateNext(double u) override;
    void reset() override;
};