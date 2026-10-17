#pragma once
#include "Model.h"

class Model2_3 : public Model {
private:
    double a1, b1, c1;
    double y_prev1 = 0.0;
public:
    Model2_3(double a1, double b1, double c1);
    double calculateNext(double u) override;
    void reset() override;
};
