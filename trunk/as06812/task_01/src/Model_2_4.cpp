#include "Model_2_4.hpp"
#include <cmath>

using namespace std;

Model_2_4::Model_2_4(double a_val, double b_val) : a(a_val), b(b_val) {}

double Model_2_4::processStep(double u) 
{
    double y_curr = getY();
    double cos_part = a * y_curr * cos(y_curr);
    double log_part = b * log(1.0 + fabs(u));
    double y_next = cos_part + log_part;
    setY(y_next);
    return y_next;
}

void Model_2_4::resetState() 
{
    setY(0.0);
}