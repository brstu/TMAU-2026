#ifndef INPUTSIGNAL_H
#define INPUTSIGNAL_H

enum class SignalType {
    Step = 1,
    Impulse = 2,
    Harmonic = 3
};

class InputSignal {
public:
    static double getValue(SignalType type, int tau);
};

#endif