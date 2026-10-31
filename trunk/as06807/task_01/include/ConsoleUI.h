#pragma once
#include <memory>
#include <string>
#include "model.h"
#include "InputType.h"

class ConsoleUI {
public:
    void run();

private:
    int readSteps() const;
    InputType selectImpact() const;
    std::unique_ptr<Model> selectModel(std::string& name);
    double readDouble(const std::string& prompt) const;
    int readChoice(int min, int max) const;
};
