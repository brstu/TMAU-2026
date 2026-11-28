#pragma once

#include <vector>

#include "Excitation.h"
#include "Model.h"

// Записанный переходный процесс: input[i] подан на такте i, output[i] получен после него
struct Trace {
    std::vector<double> input;
    std::vector<double> output;
};

// Возвращает модель в начальное состояние и прогоняет horizon тактов.
// Зависит только от абстракций Model и Excitation.
Trace simulateSteps(Model& plant, const Excitation& source, int horizon);
