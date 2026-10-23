#pragma once
#include "Model.h"
#include <cmath>

class Model3_9 : public Model {
private:
    double b;
    double y_current;
public:
    Model3_9(double b_val, double y0 = 0.0);
    double step(double u_tau) override;
    double getY() const override { return y_current; }
    void reset(double y0) override; // <-- Убрали { y_current = y0; }
    std::string getName() const override { return "Model 3.9 - Bounded Saturation Rate"; }
};