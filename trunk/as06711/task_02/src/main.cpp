// ============================================================================
// Лабораторная работа №2, вариант 19.
// Приложение собирает коэффициенты выбранной модели, прогоняет её через
// генератор входа и печатает траекторию. Дополнительно пишет CSV.
// ============================================================================
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "CsvWriter.h"
#include "LinearModel19.h"
#include "NonlinearModel23.h"
#include "ODE37.h"
#include "SignalGenerator.h"
#include "Simulator.h"
#include "StabilityAnalyzer.h"

namespace app {

    struct Options {
        int modelId = 1;
        int horizon = 10;
        double alpha = 1.0;
        double beta = 1.0;
        InputType wave = InputType::STEP;
        std::string csvPath = "doc/lab2_trace.csv";
    };

    std::unique_ptr<Model> buildModel(const Options& opt) {
        switch (opt.modelId) {
        case 1:
            return std::make_unique<LinearModel19>(
                0.4 * opt.alpha, 0.3 * opt.alpha, 0.1 * opt.alpha, opt.beta);
        case 2:
            return std::make_unique<NonlinearModel23>(opt.alpha, 0.1, opt.beta);
        case 3:
            return std::make_unique<ODE37>(opt.alpha, opt.beta, 0.1);
        default:
            return nullptr;
        }
    }

    InputType parseWave(int code) {
        switch (code) {
        case 2:  return InputType::IMPULSE;
        case 3:  return InputType::HARMONIC;
        default: return InputType::STEP;
        }
    }

    void printVector(const std::vector<double>& ys) {
        std::cout << '[';
        for (std::size_t i = 0; i < ys.size(); ++i) {
            if (i) std::cout << "; ";
            std::cout << ys[i];
        }
        std::cout << "]\n";
    }

} // namespace app

int main() {
    app::Options opts;

    std::cout << "Model id (1=Linear19, 2=Nonlinear23, 3=ODE37): ";
    std::cin >> opts.modelId;
    std::cout << "Horizon: ";
    std::cin >> opts.horizon;
    std::cout << "alpha, beta: ";
    std::cin >> opts.alpha >> opts.beta;

    std::cout << "Wave (1=step, 2=impulse, 3=harmonic): ";
    int waveCode = 1;
    std::cin >> waveCode;
    opts.wave = app::parseWave(waveCode);

    if (opts.modelId == 1) {
        StabilityAnalyzer::warnIfUnstable(
            0.4 * opts.alpha, 0.3 * opts.alpha, 0.1 * opts.alpha);
    }

    auto mdl = app::buildModel(opts);
    if (!mdl) {
        std::cerr << "Unknown model id: " << opts.modelId << '\n';
        return EXIT_FAILURE;
    }

    SignalGenerator gen(opts.wave, 1.0);
    Simulator engine(std::move(mdl), opts.horizon);
    const std::vector<double> trace = engine.run(gen);

    app::printVector(trace);
    io::writeCsv(opts.csvPath, "y", trace);

    return EXIT_SUCCESS;
}