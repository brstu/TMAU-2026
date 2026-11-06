#pragma once
#include "Model.h"

class Model2_3 : public Model {
private:
    double a1;
    double b1;
    double c1;
    double y_prev1 = 0.0;
public:
    Model2_3(double a1_val, double b1_val, double c1_val);
    double calculateNext(double u) override;
    void reset() override;
};