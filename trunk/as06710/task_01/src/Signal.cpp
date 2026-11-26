#include "Signal.h"
#include <cmath>

double getInput(SignalType type, int tau, double amplitude)
{
    switch (type)
    {
    case SignalType::Step:     return amplitude;
    case SignalType::Impulse:  return (tau == 0) ? amplitude : 0.0;
    case SignalType::Harmonic: return amplitude * std::sin(static_cast<double>(tau));
    }
    return 0.0;
}