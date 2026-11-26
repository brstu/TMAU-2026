#pragma once
#include "Model.h"
#include "ISignalGenerator.h"
#include <vector>
#include <memory>

class Simulator {
public:
    Simulator(std::unique_ptr<Model> model, int n)
        : model_(std::move(model)), n_(n) {}

    std::vector<double> run(const ISignalGenerator& signal) {
        model_->reset();
        std::vector<double> y;
        y.reserve(n_);
        for (int tau = 0; tau < n_; ++tau) {
            y.push_back(model_->step(signal.get(tau)));
        }
        return y;
    }

private:
    std::unique_ptr<Model> model_;
    int n_;
};