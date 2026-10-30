#include "impact.h"
#include <cmath>
#include <stdexcept>

std::vector<double> getInput(int steps, InputType type)
{
    if (steps <= 0)
        throw std::invalid_argument("Number of steps must be positive");

    std::vector<double> u(steps, 0.0);

    switch (type)
    {
    case InputType::Step:
        u.assign(steps, 1.0);
        break;
    case InputType::Impulse:
        u[0] = 1.0;
        break;
    case InputType::Harmonic:
        for (int t = 0; t < steps; ++t)
            u[t] = std::sin(t);
        break;
    }
    return u;
}