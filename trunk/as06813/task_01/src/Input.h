#ifndef INPUT_H
#define INPUT_H

#include <cstdlib>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>

// Ввод целого числа с проверкой диапазона.
// Если ввод закончился (EOF), программа завершается.
inline int readInt(const std::string& prompt, int minValue, int maxValue) {
    int value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value && value >= minValue && value <= maxValue) {
            std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
            return value;
        }
        if (std::cin.eof()) {
            std::cout << "\nВвод завершён.\n";
            std::exit(0);
        }
        std::cin.clear();
        std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
        std::cout << "Ошибка: нужно целое число от " << minValue << " до " << maxValue << "\n";
    }
}

// Ввод коэффициента модели. Пустая строка (Enter) - значение по умолчанию.
inline double readCoef(const std::string& name, double defaultValue,
                       double minValue = -1000.0, double maxValue = 1000.0) {
    while (true) {
        std::cout << "  " << name << " [" << defaultValue << "]: ";
        std::string line;
        if (!std::getline(std::cin, line)) {
            std::cout << "\nВвод завершён.\n";
            std::exit(0);
        }
        if (line.empty()) {
            return defaultValue;
        }

        std::istringstream in(line);
        double value;
        if (std::string rest; in >> value && !(in >> rest) && value >= minValue && value <= maxValue) {
            return value;
        }
        std::cout << "  Ошибка: нужно число от " << minValue << " до " << maxValue
                  << " (дробная часть через точку)\n";
    }
}

#endif
