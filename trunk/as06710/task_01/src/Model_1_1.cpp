#include "Model_1_1.h"

Model1_1::Model1_1(double a, double b)
    : a(a), b(b)
{
}

double Model1_1::calculateNext(
    double y,
    double u,
    double /*dt*/
) const
{
    return a * y + b * u;
}

const char* Model1_1::getName() const
{
    return "Model 1.1 - Standard First-Order Inertial System";
}