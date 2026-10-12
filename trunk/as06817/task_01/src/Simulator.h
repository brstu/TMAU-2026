#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "Model.h"
#include "InputSignal.h"
#include <string>
#include <filesystem>

class Simulator {
public:
    static void run(
        Model& model,
        SignalType signalType,
        int n,
        double initialY,
        const std::string& fileName
    );
};

#endif