#include "InputSignal.hpp"
#include <cmath>

using namespace std;

double InputSignal::getValue(int t) const 
{
    constexpr double STEP_AMPLITUDE = 1.0;
    constexpr double IMPULSE_AMPLITUDE = 1.0;

    switch (type_) 
    {
        case Type::STEP:
            return STEP_AMPLITUDE;
        case Type::IMPULSE:
            return (t == 0) ? IMPULSE_AMPLITUDE : 0.0;
        case Type::HARMONIC:
            return sin(static_cast<double>(t));
        default:
            return 0.0;
    }
}

string InputSignal::toString() const 
{
    switch (type_) 
    {
        case Type::STEP:
            return "Step";
        case Type::IMPULSE:
            return "Impulse";
        case Type::HARMONIC:
            return "Harmonic";
        default:
            return "Unknown";
    }
}