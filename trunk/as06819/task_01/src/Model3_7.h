#pragma once
#include "Model.h"

class Model3_7 : public Model {
private:
    double a, b, dt;
public:
    Model3_7(double a, double b, double dt);
    double calculateNext(double u) override;
    void reset() override;
};
