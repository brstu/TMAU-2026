#ifndef INPUT_SIGNAL_H
#define INPUT_SIGNAL_H

// Типы входных воздействий, подаваемых на модели
enum class InputType {
    STEP,       // ступенчатый сигнал: u = const = 1
    IMPULSE,    // импульс: u(0) = 1, u(t>0) = 0
    HARMONIC    // гармонический сигнал: u = sin(t)
};

// Вычисляет значение входного сигнала в момент времени timeIndex
double getInputValue(InputType signalKind, int timeIndex);

#endif // INPUT_SIGNAL_H
