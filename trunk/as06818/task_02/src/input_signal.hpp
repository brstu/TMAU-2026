#ifndef TASK02_AS06818_INPUT_SIGNAL_HPP
#define TASK02_AS06818_INPUT_SIGNAL_HPP

// Тип входного воздействия u_tau (три типа по заданию).
enum class SignalType {
    Step,     // ступенчатое: u_tau = 1
    Impulse,  // импульсное: u_0 = 1, u_tau>0 = 0
    Harmonic  // гармоническое: u_tau = sin(tau)
};

// Значение входного воздействия заданного типа на шаге tau.
double input_value(SignalType type, int tau);

// Человекочитаемое название типа воздействия для консоли и CSV.
const char* signal_name(SignalType type);

#endif
