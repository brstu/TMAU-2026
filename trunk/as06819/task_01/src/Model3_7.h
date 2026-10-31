#pragma once
#include "Model.h"

class Model3_7 : public Model {
private:
    double a;
    double b;
    double dt;
public:
    Model3_7(double a_val, double b_val, double dt_val);
    double calculateNext(double u) override;
    void reset() override;
};