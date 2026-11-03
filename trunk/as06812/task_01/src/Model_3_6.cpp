#include "Model_3_6.hpp"

using namespace std;

Model_3_6::Model_3_6(double a_val, double b_val, double dt_val) 
    : a(a_val), b(b_val), dt(dt_val) {}

double Model_3_6::processStep(double u) 
{
    double y_curr = getY();
    double y3 = y_curr * y_curr * y_curr;
    double dydt = a * y3 + b * u;
    double y_next = y_curr + dt * dydt;
    setY(y_next);
    return y_next;
}

void Model_3_6::resetState() 
{
    setY(0.0);
}