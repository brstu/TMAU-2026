#pragma once

enum class InputType {
    STEP,
    IMPULSE,
    HARMONIC
};

double getInputValue(InputType type, int t);
