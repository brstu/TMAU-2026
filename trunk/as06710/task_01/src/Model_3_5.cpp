#include "Model_3_5.h"
#include <cmath>

Model3_5::Model3_5(double b)
    : b(b)
{
}

double Model3_5::calculateNext(
    double y,
    double u,
    double dt
) const
{
    return y + dt * b * std::sin(u);
}

const char* Model3_5::getName() const
{
    return "Model 3.5 - Harmonic Driving Force";
}