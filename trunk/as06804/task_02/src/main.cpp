#include <algorithm>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

#include "DiffEqModel.h"
#include "LinearModel.h"
#include "NonLinearModel.h"
#include "InputSignal.h"
#include "SimulationRunner.h"
#include "StabilityAnalyzer.hpp"

namespace {

std::string makeCsvName(const std::string& model_name, int signal_type) {
    std::string name = model_name;
    std::replace(name.begin(), name.end(), ' ', '_');
    std::replace(name.begin(), name.end(), '.', '_');
    return name + "_signal" + std::to_string(signal_type) + ".csv";
}

} // namespace

int main() {
#ifdef _WIN32
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
#endif

    int model_choice = 0;
    int signal_choice = 0;
    int steps = 0;
    double y0 = 0.0;
    double dt = 0.0;
    double a = 0.0;
    double b = 0.0;
    double a1 = 0.0;
    double a2 = 0.0;

    std::cout << "=== LAB 2: VARIANT 4 (as06804) ===\n";
    std::cout << "Select model:\n"
              << "  1 - Linear model 1.4\n"
              << "  2 - Non-linear model 2.6\n"
              << "  3 - Differential model 3.8 (Euler)\n"
              << "Choice (1-3): ";
    if (!(std::cin >> model_choice)) {
        return 1;
    }

    if (model_choice == 1) {
        std::cout << "Enter a: ";
        std::cin >> a;
        std::cout << "Enter b: ";
        std::cin >> b;

        // Required by the assignment: warn BEFORE the simulation starts.
        const stability::AnalysisResult analysis = stability::analyzeLinearModel14(a, b);
        std::cout << analysis.message << '\n';
        if (!analysis.stable) {
            std::cout << "WARNING: the given coefficients produce an unstable "
                         "(or marginal) process. Simulation results may diverge.\n";
        }
    } else if (model_choice == 2 || model_choice == 3) {
        std::cout << "Enter a1: ";
        std::cin >> a1;
        std::cout << "Enter a2: ";
        std::cin >> a2;
        std::cout << "Enter b: ";
        std::cin >> b;
    } else {
        std::cout << "Invalid model choice.\n";
        return 1;
    }

    std::cout << "Select input signal:\n"
              << "  1 - Step\n"
              << "  2 - Impulse\n"
              << "  3 - Harmonic\n"
              << "Choice (1-3): ";
    if (!(std::cin >> signal_choice)) {
        return 1;
    }

    std::cout << "Number of steps: ";
    std::cin >> steps;
    std::cout << "Initial value Y(0): ";
    std::cin >> y0;
    std::cout << "Time step dt: ";
    std::cin >> dt;

    if (steps <= 0 || dt <= 0.0) {
        std::cout << "steps and dt must be positive.\n";
        return 1;
    }

    std::unique_ptr<Model> model;
    std::string model_name;
    if (model_choice == 1) {
        model = std::make_unique<LinearModel>(a, b, y0, dt);
        model_name = "Linear_1_4";
    } else if (model_choice == 2) {
        model = std::make_unique<NonLinearModel>(a1, a2, b, y0, dt);
        model_name = "NonLinear_2_6";
    } else {
        model = std::make_unique<DiffEqModel>(a1, a2, b, y0, dt);
        model_name = "DiffEq_3_8";
    }

    std::unique_ptr<Signal> signal;
    try {
        signal = makeSignal(signal_choice);
    } catch (const std::invalid_argument&) {
        std::cout << "Invalid signal choice.\n";
        return 1;
    }

    std::ostringstream table;
    const SimulationRecord record = runSimulation(*model, *signal, steps, dt, table);
    std::cout << '\n' << table.str();

    if (record.diverged) {
        std::cout << "WARNING: divergence detected during simulation.\n";
    }

    if (const std::string csv_name = makeCsvName(model_name, signal_choice);
        writeCsv(csv_name, record)) {
        std::cout << "Data saved to " << csv_name << '\n';
    } else {
        std::cout << "Could not write " << csv_name << '\n';
    }

    return 0;
}
