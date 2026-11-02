#include "Model_2_3.h"

Model2_3::Model2_3(double a, double b, double delta)
    : a(a), b(b), delta(delta)
{
}

double Model2_3::deadzone(double u) const
{
    if (u > delta)  return u - delta;
    if (u < -delta) return u + delta;
    return 0.0;
}

double Model2_3::calculateNext(
    double y,
    double u,
    double /*dt*/
) const
{
    return a * y + b * deadzone(u);
}

const char* Model2_3::getName() const
{
    return "Model 2.3 - Dead-Zone Non-linearity";
}