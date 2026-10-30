#pragma once
#include <memory>
#include <string>
#include "model.h"
#include "InputType.h"

class ConsoleUI {
public:
    void run();

private:
    int readSteps();
    InputType selectImpact();
    std::unique_ptr<Model> selectModel(std::string& name);
    double readDouble(const std::string& prompt);
    int readChoice(int min, int max);
};