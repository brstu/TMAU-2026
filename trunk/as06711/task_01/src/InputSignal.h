#ifndef INPUT_SIGNAL_H
#define INPUT_SIGNAL_H

// Типы входных воздействий, подаваемых на модели
enum class InputType {
    STEP,       // ступенчатый сигнал
    IMPULSE,    // импульс в момент t = 0
    HARMONIC    // гармонический (синусоидальный) сигнал
};

// Вычисляет значение входного сигнала в момент времени timeIndex
double getInputValue(InputType signalKind, int timeIndex);

#endif // INPUT_SIGNAL_H