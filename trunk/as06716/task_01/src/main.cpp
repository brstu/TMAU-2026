#include <charconv>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

#include "models.hpp"

#ifndef DOC_DIR
#define DOC_DIR "doc"
#endif


namespace {

constexpr int DEFAULT_STEPS = 20;

enum class InputType { Step, Impulse, Harmonic };

std::string inputName(InputType type) {
    switch (type) {
        case InputType::Step: return "step";
        case InputType::Impulse: return "impulse";
        default: return "harmonic";
    }
}

double inputValue(InputType type, int tau) {
    switch (type) {
        case InputType::Step: return 1.0;
        case InputType::Impulse: return tau == 0 ? 1.0 : 0.0;
        default: return std::sin(static_cast<double>(tau));
    }
}

int readSteps() {
    std::cout << "Количество шагов симуляции n [" << DEFAULT_STEPS << "]: ";
    std::string line;
    std::getline(std::cin, line);

    if (line.empty()) {
        return DEFAULT_STEPS;
    }

    int n = 0;
    const char* end = line.data() + line.size();

    if (const auto result = std::from_chars(line.data(), end, n);
        result.ec == std::errc() && result.ptr == end && n > 0) {

        return n;
    }

    std::cout << "Некорректный ввод, используется " << DEFAULT_STEPS << ".\n";
    return DEFAULT_STEPS;
}

bool simulate(InputType type, int steps) {
    LinearModel16 m16(0.5, 0.2, 0.1, 1.0);
    NonlinearModel210 m210(0.8, 0.5);
    DiffModel34 m34(0.5, 0.1, 1.0);

    const std::filesystem::path docDir = DOC_DIR;
    std::filesystem::create_directories(docDir);
    const auto fileName = docDir / ("results_" + inputName(type) + ".csv");
    std::ofstream csv(fileName);
    if (!csv) {
        std::cerr << "Не удалось открыть файл " << fileName << "\n";

        return false;
    }

    std::cout << "\nВходное воздействие: " << inputName(type) << "\n" 
        << std::setw(5) << "tau" << std::setw(12) << "u" << std::setw(14) << "y (1.6)" << std::setw(14) << "y (2.10)" << std::setw(14) << "y (3.4)" << "\n";
    csv << "tau,u,y_1.6,y_2.10,y_3.4\n";
    std::cout << std::fixed << std::setprecision(6);
    csv << std::fixed << std::setprecision(6);

    for (int tau = 0; tau <= steps; ++tau) {
        const double u = inputValue(type, tau);
        std::cout << std::setw(5) << tau << std::setw(12) << u << std::setw(14)
            << m16.output() << std::setw(14) << m210.output() << std::setw(14) << m34.output() << "\n";
        csv << tau << ',' << u << ',' << m16.output() << ',' << m210.output() << ',' << m34.output() << '\n';
        m16.step(u);
        m210.step(u);
        m34.step(u);
    }

    return true;
}

}  // namespace

int main() {
    const int steps = readSteps();
    const bool ok = simulate(InputType::Step, steps) && simulate(InputType::Impulse, steps) && simulate(InputType::Harmonic, steps);
    
    return ok ? 0 : 1;
}
