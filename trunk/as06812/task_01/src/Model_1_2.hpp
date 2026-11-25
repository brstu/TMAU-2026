#pragma once
#include "Model.hpp"
#include <queue>

class Model_1_2 : public Model 
{
private:
    double a;
    double b;
    int k;
    std::queue<double> u_history;

public:
    Model_1_2(double a_val, double b_val, int k_val);
    double processStep(double u) override;
    void resetState() override;
};