#include <charconv>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>

#include "models.hpp"

#ifndef DOC_DIR
#define DOC_DIR "doc"
#endif


namespace {

constexpr int DEFAULT_STEPS = 20;
constexpr int MAX_STEPS = 100000;
constexpr int MODEL_COUNT = 3;
constexpr int INPUT_COUNT = 3;

enum class InputType { Step = 1, Impulse = 2, Harmonic = 3 };

std::string inputName(InputType type) {
    switch (type) {
        case InputType::Step: return "Step";
        case InputType::Impulse: return "Impulse";
        default: return "Harmonic";
    }
}

double inputValue(InputType type, int tau) {
    switch (type) {
        case InputType::Step: return 1.0;
        case InputType::Impulse: return tau == 0 ? 1.0 : 0.0;
        default: return std::sin(static_cast<double>(tau));
    }
}

template <typename T>
bool parseValue(std::string_view text, T& value) {
    const char* end = text.data() + text.size();
    const auto [ptr, ec] = std::from_chars(text.data(), end, value);

    return ec == std::errc() && ptr == end;
}

double readDouble(const std::string& name, double defaultValue) {
    std::cout << "  " << name << " [" << defaultValue << "]: ";
    std::string line;
    if (!std::getline(std::cin, line) || line.empty()) {
        return defaultValue;
    }
    if (double value = 0.0; parseValue(line, value) && std::isfinite(value)) {
        return value;
    }
    std::cout << "  Некорректный ввод, используется " << defaultValue << ".\n";

    return defaultValue;
}

double readPositive(const std::string& name, double defaultValue) {
    if (const double value = readDouble(name, defaultValue); value > 0.0) {
        return value;
    }
    std::cout << "  Значение должно быть больше нуля, используется " << defaultValue << ".\n";

    return defaultValue;
}

int readChoice(const std::string& prompt, int maxValue) {
    std::cout << prompt;
    std::string line;
    while (std::getline(std::cin, line)) {
        if (int choice = 0; parseValue(line, choice) && choice >= 1 && choice <= maxValue) {
            return choice;
        }
        std::cout << "Введите число от 1 до " << maxValue << ": ";
    }

    return 1;
}

int readSteps() {
    std::cout << "Количество шагов симуляции n [" << DEFAULT_STEPS << "]: ";
    std::string line;
    if (!std::getline(std::cin, line) || line.empty()) {
        return DEFAULT_STEPS;
    }

    if (int n = 0; parseValue(line, n) && n > 0 && n <= MAX_STEPS) {
        return n;
    }

    std::cout << "Введите целое число от 1 до " << MAX_STEPS << ", используется " << DEFAULT_STEPS << ".\n";
    return DEFAULT_STEPS;
}

std::unique_ptr<Model> createModel(int choice) {
    std::cout << "\nКоэффициенты (enter - значение по умолчанию):\n";
    if (choice == 1) {
        const double a1 = readDouble("a1", 0.5);
        const double a2 = readDouble("a2", 0.2);
        const double a3 = readDouble("a3", 0.1);
        const double b = readDouble("b", 1.0);

        return std::make_unique<LinearModel16>(a1, a2, a3, b);
    }
    if (choice == 2) {
        const double a = readDouble("a", 0.8);
        const double b = readDouble("b", 0.5);

        return std::make_unique<NonlinearModel210>(a, b);
    }
    const double a = readDouble("a", 0.5);
    const double dt = readPositive("dt", 0.1);
    const double y0 = readDouble("y0", 1.0);
    
    return std::make_unique<DiffModel34>(a, dt, y0);
}


bool simulate(Model& model, InputType type, int steps) {
    const std::filesystem::path docDir = DOC_DIR;
    std::filesystem::create_directories(docDir);
    const auto fileName = docDir / "results.csv";
    std::ofstream csv(fileName);
    if (!csv) {
        std::cerr << "Не удалось открыть файл " << fileName << "\n";
      
        return false;
    }

    std::cout << "\nМодель: " << model.name() << ", воздействие: " << inputName(type) << "\n"
        << std::setw(6) << "tau" << std::setw(14) << "u" << std::setw(14) << "y" << "\n";
    csv << "model,input,tau,u,y\n";
    std::cout << std::fixed << std::setprecision(6);
    csv << std::fixed << std::setprecision(6);

    for (int tau = 1; tau <= steps; ++tau) {
        const double u = inputValue(type, tau - 1);
        const double y = model.step(u);
        std::cout << std::setw(6) << tau << std::setw(14) << u << std::setw(14) << y << "\n";
        csv << model.name() << ',' << inputName(type) << ',' << tau << ',' << u << ',' << y << '\n';
    }

    return true;
}

}  // namespace

int main() {
    const int modelChoice = readChoice("Модель: 1 - Model 1.6, 2 - Model 2.10, 3 - Model 3.4. Выбор: ", MODEL_COUNT);
    const int inputChoice = readChoice("Воздействие: 1 - ступенчатое, 2 - импульсное, 3 - гармоническое. Выбор: ", INPUT_COUNT);
    const auto model = createModel(modelChoice);
    const int steps = readSteps();

    return simulate(*model, static_cast<InputType>(inputChoice), steps) ? 0 : 1;
}
