#pragma once
#include "model.h"

class InputSignal {
public:
    static double getSignal(InputType type, int step, double amp);
};