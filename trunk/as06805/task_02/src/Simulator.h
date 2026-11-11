#pragma once

#include "Model.h"
#include "SignalGenerator.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

// Управляющий класс симуляции: прогоняет модель по заданному входному сигналу.
class Simulator {
public:
    Simulator(std::unique_ptr<Model> modelInstance, int stepsCount)
        : targetModel_(std::move(modelInstance)), totalSteps_(stepsCount) {}

    // Выполняет симуляцию и возвращает историю выходного сигнала.
    std::vector<double> run(const ISignalSource& signal);

    // Печать таблицы результатов (по умолчанию — в консоль).
    void printTable(const std::vector<double>& results, const ISignalSource& signal,
                    std::ostream& out = std::cout) const;

    // Экспорт результатов в CSV-файл.
    void exportCSV(const std::vector<double>& results, const ISignalSource& signal,
                   const std::string& filePath) const;

    std::string modelName() const;
    int stepsCount() const noexcept { return totalSteps_; }
    bool hasModel() const noexcept { return targetModel_ != nullptr; }

private:
    std::unique_ptr<Model> targetModel_;
    int totalSteps_;
};
