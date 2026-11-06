#pragma once

enum class SignalType
{
    Step = 1,
    Impulse = 2,
    Harmonic = 3
};

double getInput(SignalType type, int tau, double amplitude);