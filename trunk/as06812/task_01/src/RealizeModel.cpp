#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <locale>
#include <memory>
#include <string>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

#include "models.h"

namespace {

// Макс кол-во шагов
constexpr int kMaxSteps = 100000;
// Порог после которого значения ломаются
constexpr double kDivergenceLimit = 1.0e9;


bool readNumber(const std::string& prompt, double& value) {
    std::string line;
    while (std::cout << prompt && std::getline(std::cin, line)) {
        char* end = nullptr;
        const double parsed = std::strtod(line.c_str(), &end);
        const bool hasNumber = end != line.c_str();
        const bool onlySpacesLeft = std::string(end).find_first_not_of(" \t\r") == std::string::npos;
        if (hasNumber && onlySpacesLeft && std::isfinite(parsed)) {
            value = parsed;
            return true;
        }
        std::cout << "Ошибка: нужно ввести число.\n";
    }
    return false;
}

bool readInt(const std::string& prompt, int low, int high, int& result) {
    double value = 0.0;
    while (readNumber(prompt, value)) {
        const bool isWhole = std::abs(value - std::round(value)) < 1e-9;
        if (isWhole && value >= low && value <= high) {
            result = static_cast<int>(std::lround(value));
            return true;
        }
        std::cout << "Ошибка: нужно целое число от " << low << " до " << high << ".\n";
    }
    return false;
}

std::unique_ptr<Model> createModel1_10() {
    double a1 = 0.0;
    double b1 = 0.0;
    double b2 = 0.0;
    std::cout << "\nНастройка модели 1.10\n";
    if (!readNumber("Введите коэффициент a1: ", a1) ||
        !readNumber("Введите коэффициент b1: ", b1) ||
        !readNumber("Введите коэффициент b2: ", b2)) {
        return nullptr;
    }
    return std::make_unique<Model1_10>(a1, b1, b2);
}

std::unique_ptr<Model> createModel2_2() {
    double a = 0.0;
    double b = 0.0;
    double uMin = 0.0;
    double uMax = 0.0;
    std::cout << "\nНастройка модели 2.2\n";
    if (!readNumber("Введите коэффициент a: ", a) ||
        !readNumber("Введите коэффициент b: ", b) ||
        !readNumber("Введите нижний предел насыщения Umin: ", uMin) ||
        !readNumber("Введите верхний предел насыщения Umax: ", uMax)) {
        return nullptr;
    }
    if (uMax < uMin) {
        std::cout << "Ошибка: Umax не может быть меньше Umin.\n";
        return nullptr;
    }
    return std::make_unique<Model2_2>(a, b, uMin, uMax);
}

std::unique_ptr<Model> createModel3_4() {
    double a = 0.0;
    double deltaT = 0.0;
    double y0 = 0.0;
    std::cout << "\nНастройка модели 3.4\n";
    if (!readNumber("Введите коэффициент a: ", a) ||
        !readNumber("Введите шаг времени deltaT: ", deltaT) ||
        !readNumber("Введите начальное значение y0: ", y0)) {
        return nullptr;
    }
    if (deltaT <= 0.0) {
        std::cout << "Ошибка: шаг времени должен быть больше нуля.\n";
        return nullptr;
    }
    return std::make_unique<Model3_4>(a, deltaT, y0);
}

}  // namespace

int main() {
#ifdef _WIN32
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
#endif

    int steps = 0;
    int modelChoice = 0;
    int signalChoice = 0;

    if (!readInt("Введите количество шагов моделирования n: ", 1, kMaxSteps, steps)) {
        return 1;
    }

    std::cout << "\nВыберите модель:\n"
              << "1 - Модель 1.10 (линейная)\n"
              << "2 - Модель 2.2 (нелинейная, насыщение)\n"
              << "3 - Модель 3.4 (дифференциальная)\n";
    if (!readInt("Ваш выбор: ", 1, 3, modelChoice)) {
        return 1;
    }

    std::unique_ptr<Model> model;
    if (modelChoice == 1) {
        model = createModel1_10();
    } else if (modelChoice == 2) {
        model = createModel2_2();
    } else {
        model = createModel3_4();
    }
    if (!model) {
        return 1;
    }

    std::cout << "\nВыберите входное воздействие:\n"
              << "1 - Ступенчатое\n"
              << "2 - Импульсное\n"
              << "3 - Гармоническое\n";
    if (!readInt("Ваш выбор: ", 1, 3, signalChoice)) {
        return 1;
    }
    const auto signal = static_cast<SignalType>(signalChoice);

    std::ofstream csv("result.csv", std::ios::out | std::ios::trunc);
    if (!csv) {
        std::cout << "Ошибка: не удалось создать result.csv.\n";
        return 1;
    }
    csv.imbue(std::locale::classic());
    csv << "step,u_tau,y_next,model,signal\n" << std::setprecision(15);

    std::cout << "\nМодель: " << model->title() << "\n\n";
    // "Шаг" занимает 6 байт в UTF-8, поэтому ширина 11 даёт 8 символов на экране
    std::cout << std::setw(11) << "Шаг" << std::setw(16) << "u(tau)" << std::setw(16) << "y(tau+1)" << '\n'
              << std::string(40, '-') << '\n'
              << std::fixed << std::setprecision(4);

    int done = 0;
    for (int tau = 0; tau < steps; ++tau) {
        const double u = signalValue(signal, tau);
        const double y = model->nextValue(u);

        if (!std::isfinite(y) || std::fabs(y) > kDivergenceLimit) {
            std::cout << "\nОшибка: расчёт расходится на шаге " << tau + 1
                      << ". Показаны только корректные шаги.\n";
            break;
        }

        std::cout << std::setw(8) << tau + 1 << std::setw(16) << u << std::setw(16) << y << '\n';
        csv << tau + 1 << ',' << u << ',' << y << ',' << model->csvName() << ','
            << signalName(signal) << '\n';
        ++done;
    }
    csv.close();

    if (done == 0) {
        std::cout << "Ошибка: результатов нет.\n";
        return 1;
    }
    std::cout << "\nРезультаты сохранены в result.csv\n";
    return 0;
}