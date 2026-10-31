#pragma once
#include "Model.h"
#include "SignalGenerator.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <memory>

class Simulator {
public:
    Simulator(std::unique_ptr<Model> model, int n)
        : model_(std::move(model)), n_(n) {}

    // запуск с одним сигналом, возврат вектора y
    std::vector<double> run(const SignalGenerator& signal) {
        model_->reset();
        std::vector<double> y;
        y.reserve(n_);
        for (int tau = 0; tau < n_; ++tau) {
            double u = signal.get(tau);
            double yNext = model_->step(u);
            y.push_back(yNext);
        }
        return y;
    }

    // вывод в консоль в табличном виде
    void printTable(const std::vector<double>& y,
                    const SignalGenerator& signal) const {
        std::cout << std::setw(6) << "tau"
                  << std::setw(15) << "u(tau)"
                  << std::setw(15) << "y(tau)" << "\n";
        std::cout << std::string(36, '-') << "\n";
        for (size_t tau = 0; tau < y.size(); ++tau) {
            std::cout << std::setw(6) << tau
                      << std::setw(15) << std::fixed << std::setprecision(4)
                      << signal.get((double)tau)
                      << std::setw(15) << y[tau] << "\n";
        }
    }

    // экспорт в CSV
    void exportCSV(const std::vector<double>& y,
                   const SignalGenerator& signal,
                   const std::string& filename) const {
        std::ofstream f(filename);
        f << "tau,u,y\n";
        for (size_t tau = 0; tau < y.size(); ++tau) {
            f << tau << ","
              << signal.get((double)tau) << ","
              << y[tau] << "\n";
        }
        f.close();
    }

    const char* modelName() const { return model_->name(); }

private:
    std::unique_ptr<Model> model_;
    int n_;
};
