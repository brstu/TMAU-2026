#include "InputSignal.h"
#include <cmath> // Для функции std::sin

double getInputValue(InputType type, int t) {
    switch (type) {
        case InputType::STEP:
            return 1.0; // Ступенчатый сигнал всегда равен 1
            
        case InputType::IMPULSE:
            return (t == 0) ? 1.0 : 0.0; // Импульс только в момент t = 0
            
        case InputType::HARMONIC:
            return std::sin(static_cast<double>(t)); // Синусоида
            
        default:
            return 0.0;
    }
}