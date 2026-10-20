#pragma once
#include "Model.h"

class Model1_9 : public Model {
private:
    double a1;
    double a2;
    double a3;
    double b1;
    double y_prev1 = 0.0;
    double y_prev2 = 0.0;
public:
    Model1_9(double a1_val, double a2_val, double a3_val, double b1_val);
    double calculateNext(double u) override;
    void reset() override;
};