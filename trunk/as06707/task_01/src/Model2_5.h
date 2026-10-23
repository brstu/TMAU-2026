#pragma once
#include "Model.h"
#include <cmath>

class Model2_5 : public Model {
private:
    double a, b;
    double y_current;
public:
    Model2_5(double a_val, double b_val, double y0 = 0.0);
    double step(double u_tau) override;
    double getY() const override { return y_current; }
    void reset(double y0) override; // <-- Убрали { y_current = y0; }
    std::string getName() const override { return "Model 2.5 - Signum Friction and Exponential Growth"; }
};