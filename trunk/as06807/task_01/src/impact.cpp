#include "impact.h"
#include <cmath>
#include <stdexcept>

std::vector<ImpactResult> getInput(int steps)
{
    if (steps <= 0)
    {
        throw std::invalid_argument(
            "Number of steps must be positive"
        );
    }

    std::vector<ImpactResult> u(3);

    // Ступенчатое воздействие.
    u[0].type = InputType::Step;
    u[0].values.resize(steps, 1.0);

    // Импульсное воздействие.
    u[1].type = InputType::Impulse;
    u[1].values.resize(steps, 0.0);
    u[1].values[0] = 1.0;

    // Гармоническое воздействие.
    u[2].type = InputType::Harmonic;
    u[2].values.resize(steps);

    for (int tau = 0; tau < steps; ++tau)
    {
        u[2].values[tau] = std::sin(tau);
    }

    return u;
}