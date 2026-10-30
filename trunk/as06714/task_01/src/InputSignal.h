#pragma once

#include <cstddef>

// Виды управляющих воздействий u(τ), подаваемых на вход моделей
enum class InputType : unsigned char {
    STEP,      // постоянный уровень
    IMPULSE,   // единичный импульс в начальный момент
    HARMONIC   // синусоида от номера такта
};

// Сколько видов воздействий поддерживает программа
inline constexpr std::size_t kInputTypeCount = 3;

// Значение воздействия вида type на такте tau (для неизвестного вида — 0)
double getInputValue(InputType type, int tau);
