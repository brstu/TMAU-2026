#ifndef INPUT_SIGNAL_H
#define INPUT_SIGNAL_H

// Типы входных воздействий, подаваемых на модели
enum class InputType {
    STEP,       // ступенчатый сигнал, постоянная единица
    IMPULSE,    // единичный импульс в начальный момент времени
    HARMONIC    // гармонический сигнал, синус от номера шага
};

// Вычисляет значение входного сигнала в момент времени timeIndex
double getInputValue(InputType signalKind, int timeIndex);

#endif // INPUT_SIGNAL_H
