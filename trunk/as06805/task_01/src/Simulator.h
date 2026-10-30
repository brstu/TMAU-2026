#pragma once

#include "Model.h"
#include "SignalGenerator.h"
#include <vector>
#include <memory>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <utility>

class Simulator {
private:
    std::unique_ptr<Model> targetModel_;
    int totalSteps_;

public:
    Simulator(std::unique_ptr<Model> modelInstance, int stepsCount)
        : targetModel_(std::move(modelInstance)), totalSteps_(stepsCount) {}

    std::vector<double> run(const SignalGenerator& signalGen) {
        if (!targetModel_) return {};

        targetModel_->reset();
        std::vector<double> outputHistory;
        outputHistory.reserve(totalSteps_);

        for (int stepIdx = 0; stepIdx < totalSteps_; ++stepIdx) {
            double currentInput = signalGen.get(static_cast<double>(stepIdx));
            double currentOutput = targetModel_->step(currentInput);
            outputHistory.push_back(currentOutput);
        }

        return outputHistory;
    }

    void printTable(const std::vector<double>& results, const SignalGenerator& signalGen) const {
        std::cout << "\n=========================================\n";
        std::cout << " Модель: " << (targetModel_ ? targetModel_->name() : "N/A") << "\n";
        std::cout << "=========================================\n";
        std::cout << std::setw(8) << "Шаг (tau)" 
                  << std::setw(12) << "Вход u(t)" 
                  << std::setw(15) << "Выход y(t)\n";
        std::cout << "-----------------------------------------\n";

        for (size_t i = 0; i < results.size(); ++i) {
            double uVal = signalGen.get(static_cast<double>(i));
            std::cout << std::setw(8) << i 
                      << std::setw(12) << std::fixed << std::setprecision(4) << uVal 
                      << std::setw(15) << std::fixed << std::setprecision(4) << results[i] << "\n";
        }
        std::cout << "-----------------------------------------\n";
    }

    void exportCSV(const std::vector<double>& results, const SignalGenerator& signalGen, const std::string& filePath) const {
        std::ofstream fileOut(filePath);
        if (!fileOut.is_open()) {
            std::cerr << "Ошибка: Не удалось открыть файл " << filePath << " для записи!\n";
            return;
        }

        fileOut << "tau,u,y\n";
        for (size_t i = 0; i < results.size(); ++i) {
            double uVal = signalGen.get(static_cast<double>(i));
            fileOut << i << "," << uVal << "," << results[i] << "\n";
        }
    }
};