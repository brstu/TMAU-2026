#pragma once

// Перечисление типов входных сигналов
enum class InputType {
    STEP,       // Ступенчатый
    IMPULSE,    // Импульсный
    HARMONIC    // Гармонический (синусоида)
};

// Функция для получения значения сигнала в момент времени t
double getInputValue(InputType type, int t);