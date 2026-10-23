#pragma once
#include "Model.h"
#include <cmath>

class Model1_1 : public Model {
private:
    double a, b;
    double y_current;
public:
    Model1_1(double a_val, double b_val, double y0 = 0.0);
    double step(double u_tau) override;
    double getY() const override { return y_current; }
    void reset(double y0) override; // <-- Убрали { y_current = y0; }
    std::string getName() const override { return "Model 1.1 - Standard First-Order Inertial System"; }
    bool isStable() const { return std::abs(a) < 1.0; }
};