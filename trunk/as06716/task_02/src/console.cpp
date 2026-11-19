#include "console.hpp"

#include <cmath>

#include "stability.hpp"


double Console::readDouble(const std::string& name, double defaultValue) {
    out_ << "  " << name << " [" << defaultValue << "]: ";
    std::string line;
    if (!std::getline(in_, line) || line.empty()) {
        return defaultValue;
    }
    if (double value = 0.0; parseValue(line, value) && std::isfinite(value)) {
        return value;
    }
    out_ << "  Некорректный ввод, используется " << defaultValue << ".\n";

    return defaultValue;
}

double Console::readPositive(const std::string& name, double defaultValue) {
    if (const double value = readDouble(name, defaultValue); value > 0.0) {
        return value;
    }
    out_ << "  Значение должно быть больше нуля, используется " << defaultValue << ".\n";

    return defaultValue;
}

int Console::readChoice(const std::string& prompt, int maxValue) {
    out_ << prompt;
    std::string line;
    while (std::getline(in_, line)) {
        if (int choice = 0; parseValue(line, choice) && choice >= 1 && choice <= maxValue) {
            return choice;
        }
        out_ << "Введите число от 1 до " << maxValue << ": ";
    }

    return 1;
}

int Console::readSteps() {
    out_ << "Количество шагов симуляции n [" << DEFAULT_STEPS << "]: ";
    std::string line;
    if (!std::getline(in_, line) || line.empty()) {
        return DEFAULT_STEPS;
    }

    if (int n = 0; parseValue(line, n) && n > 0 && n <= MAX_STEPS) {
        return n;
    }

    out_ << "Введите целое число от 1 до " << MAX_STEPS << ", используется " << DEFAULT_STEPS << ".\n";

    return DEFAULT_STEPS;
}

std::unique_ptr<Model> Console::createModel(int choice) {
    out_ << "\nКоэффициенты (enter - значение по умолчанию):\n";
    if (choice == 1) {
        const double a1 = readDouble("a1", 0.5);
        const double a2 = readDouble("a2", 0.2);
        const double a3 = readDouble("a3", 0.1);
        const double b = readDouble("b", 1.0);
        stability::warnIfUnstable(a1, a2, a3, out_);

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
