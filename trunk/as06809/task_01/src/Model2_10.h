#pragma once
#include "Model.h"

class Model2_10 : public Model {
private:
    double a;
    double b;
    double c;
    double d;
    double y_prev1 = 0.0;
    double u_prev1 = 0.0;
public:
    Model2_10(double p_a, double p_b, double p_c, double p_d);
    double calculateNext(double u) override;
    void reset() override;
};