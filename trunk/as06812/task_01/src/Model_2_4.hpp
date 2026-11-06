#pragma once
#include "Model.hpp"

class Model_2_4 : public Model 
{
private:
    double a;
    double b;
    
public:
    Model_2_4(double a_val, double b_val);
    double processStep(double u) override;
    void resetState() override;
};