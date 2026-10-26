#pragma once
#include "Model.hpp"

class Model_3_6 : public Model 
{
private:
    double a;
    double b;
    double dt;
    
public:
    Model_3_6(double a_val, double b_val, double dt_val);
    double processStep(double u) override;
    void resetState() override;
};