#include <array>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#ifdef _WIN32
#define NOMINMAX 
#include <windows.h>
#endif

#include "Input.h"
#include "LinearModel.h"
#include "NonlinearModel.h"
#include "DifferentialModel.h"

const std::array<std::string, 3> SIGNALS = { "step", "impulse", "harmonic" };

// Значение входного сигнала на шаге k
double inputSignal(int signal, int k) {
    if (signal == 1) {
        return 1.0;                                 // ступенчатое
    }
    if (signal == 2) {
        return k == 0 ? 1.0 : 0.0;                  // импульсное
    }
    return std::sin(static_cast<double>(k));       // гармоническое
}

// Расчёт n шагов и запись в файл; возвращает имя файла
std::string simulate(Model& model, int signal, int n) {
    std::string file = "result_" + model.code() + "_" + SIGNALS[signal - 1] + ".csv";
    std::ofstream out(file);
    out << "k;t;u;y\n";

    double dt = model.timeStep();
    model.reset();
    for (int k = 0; k <= n; k++) {
        double u = inputSignal(signal, k);
        out << k << ";" << k * dt << ";" << u << ";" << model.getOutput() << "\n";
        model.nextStep(u, dt);
    }
    return file;
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    std::vector<std::unique_ptr<Model>> models;
    models.push_back(std::make_unique<LinearModel>());
    models.push_back(std::make_unique<NonlinearModel>());
    models.push_back(std::make_unique<DifferentialModel>());

    std::cout << "Моделирование объектов управления\n";

    int again = 1;
    while (again == 1) {
        int n = readInt("\nВведите количество шагов n (1-5000): ", 1, 5000);

        std::cout << "\nМодели:\n";
        for (size_t i = 0; i < models.size(); i++) {
            std::cout << "  [" << i + 1 << "] " << models[i]->name() << "\n";
        }
        int m = readInt("Номер модели: ", 1, 3);
        Model& model = *models[m - 1];
        model.readParameters();

        std::cout << "\nВходное воздействие:\n"
                  << "  [1] ступенчатое  u = 1\n"
                  << "  [2] импульсное   u(0) = 1, далее 0\n"
                  << "  [3] гармоническое u = sin(k)\n";
        int signal = readInt("Номер воздействия: ", 1, 3);

        std::string file = simulate(model, signal, n);
        std::cout << "Расчёт закончен, данные записаны в " << file << std::endl;

        // таблица и график - в Python
        if (std::string cmd = "python plot.py " + file; std::system(cmd.c_str()) != 0) {
            std::cout << "plot.py не запустился (нужны Python и matplotlib)\n";
        }

        again = readInt("\nПовторить расчет? 1 - да, 0 - нет: ", 0, 1);
    }

    return 0;
}
