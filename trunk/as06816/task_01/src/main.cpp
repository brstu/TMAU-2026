#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

#include "DiscreteIntegrator.h"
#include "LogisticModel.h"
#include "InputDriveModel.h"

// Чтение строки; при конце ввода (EOF) программа завершается
std::string readLine(const std::string& prompt) {
    std::cout << prompt;
    std::string line;
    if (!std::getline(std::cin, line)) {
        std::cout << "\nВвод завершён. Выход.\n";
        std::exit(0);
    }
    return line;
}

int readInt(const std::string& prompt, int minValue, int maxValue) {
    while (true) {
        std::string line = readLine(prompt);
        std::istringstream in(line);
        int value;
        if (std::string rest; in >> value && !(in >> rest) && value >= minValue && value <= maxValue) {
            return value;
        }
        std::cout << "Введите целое число от " << minValue << " до " << maxValue << ".\n";
    }
}

// Ввод вещественного числа; пустая строка - значение по умолчанию
double readDouble(const std::string& name, double defaultValue, bool positive = false) {
    while (true) {
        std::ostringstream prompt;
        prompt << "  " << name << " [" << defaultValue << "]: ";
        std::string line = readLine(prompt.str());
        if (line.empty()) {
            return defaultValue;
        }
        std::istringstream in(line);
        double value;
        if (std::string rest; in >> value && !(in >> rest) && std::isfinite(value) && (!positive || value > 0.0)) {
            return value;
        }
        std::cout << (positive ? "  Введите положительное число.\n"
                               : "  Введите число (дробная часть через точку).\n");
    }
}

// Входное воздействие на шаге tau
double inputSignal(int type, int tau) {
    switch (type) {
    case 1:  return 1.0;                                // ступенчатое
    case 2:  return tau == 0 ? 1.0 : 0.0;               // импульсное
    case 3:  return std::sin(static_cast<double>(tau)); // гармоническое
    default: return 0.0;
    }
}

// Создание выбранной модели с параметрами, введёнными пользователем
std::unique_ptr<Model> createModel(int modelNumber) {
    std::cout << "\nПараметры модели (Enter - значение по умолчанию):\n";
    if (modelNumber == 1) {
        double a = readDouble("a", 0.6);
        double b = readDouble("b", 1.0);
        return std::make_unique<DiscreteIntegrator>(a, b);
    }
    if (modelNumber == 2) {
        double a = readDouble("a", 2.5);
        double b = readDouble("b", 0.3);
        double c = readDouble("c", 0.2);
        return std::make_unique<LogisticModel>(a, b, c);
    }
    double b = readDouble("b", 0.5);
    double dt = readDouble("dt", 0.1, true);
    return std::make_unique<InputDriveModel>(b, dt);
}

// Моделирование и запись результатов в CSV
bool simulate(Model& model, int signalType, int n, const std::string& fileName) {
    std::ofstream csv(fileName);
    if (!csv.is_open()) {
        std::cout << "Не удалось создать файл " << fileName << "\n";
        return false;
    }

    model.reset();
    csv << "tau,u,y\n";

    for (int tau = 0; tau <= n; tau++) {
        double u = inputSignal(signalType, tau);
        double y = model.output();
        csv << tau << "," << u << "," << y << "\n";

        if (!std::isfinite(y) || std::fabs(y) > 1e6) {
            std::cout << "Выход расходится, расчёт остановлен на шаге " << tau << "\n";
            break;
        }
        if (tau < n) {
            model.step(u);
        }
    }
    return true;
}

int main() {
#ifdef _WIN32
    // Русский текст в консоли Windows
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    std::cout <<"Моделирование объектов управления\n";

    while (true) {
        std::cout << "\nВыберите модель:\n"
                  << "  1 - линейная модель 1.4:          y(t+1) = a*y(t) + b*(u(t) - u(t-1))\n"
                  << "  2 - нелинейная модель 2.8:        y(t+1) = a*y(t)*(1 - y(t)) + b*u(t) + c*sin(y(t-1)*u(t))\n"
                  << "  3 - дифференциальная модель 3.2:  dy/dt = b*u \n"
                  << "  0 - выход\n";
        int modelNumber = readInt("Ваш выбор: ", 0, 3);
        if (modelNumber == 0) {
            break;
        }

        std::unique_ptr<Model> model = createModel(modelNumber);

        std::cout << "\nВыберите входное воздействие:\n"
                  << "  1 - ступенчатое\n"
                  << "  2 - импульсное\n"
                  << "  3 - гармоническое\n";
        int signalType = readInt("Ваш выбор: ", 1, 3);

        int n = readInt("\nКоличество шагов n (1-10000): ", 1, 10000);

        std::string fileName = "model_" + std::to_string(modelNumber)
                             + "_signal_" + std::to_string(signalType) + ".csv";

        if (!simulate(*model, signalType, n, fileName)) {
            continue;
        }
        std::cout << "\nРезультаты сохранены в файл " << fileName << std::endl;

        // Вывод таблицы и графика выполняет Python-скрипт
        if (std::string command = "python plot.py " + fileName; std::system(command.c_str()) != 0) {
            std::cout << "Не удалось запустить plot.py. Проверьте, что установлены Python и matplotlib.\n";
        }
    }

    std::cout << "\nПрограмма завершена.\n";
    return 0;
}
