// Лабораторная работа №2, вариант 15.
// Перед симуляцией выбранная модель проверяется модулем StabilityAnalyzer:
// при неустойчивых коэффициентах в консоль выводится предупреждение.

#include <cstdio>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>

#include "InputSignal.h"
#include "Model1_5.h"
#include "Model2_9.h"
#include "Model3_3.h"
#include "Simulator.h"
#include "StabilityAnalyzer.h"

namespace {

    // Читает число; при пустой или ошибочной строке остаётся значение по умолчанию
    double readParam(const char* title, double preset) {
        std::printf("%s (default %g): ", title, preset);
        std::string text;
        std::getline(std::cin, text);
        try {
            size_t used = 0;
            const double parsed = std::stod(text, &used);
            if (used > 0) {
                return parsed;
            }
        } catch (const std::exception&) {
            // некорректный ввод - берём значение по умолчанию
        }
        return preset;
    }

    int readOption(const char* title, int count) {
        const int picked = static_cast<int>(readParam(title, 1.0));
        return (picked >= 1 && picked <= count) ? picked : 1;
    }

    // Выбранная пользователем модель вместе с результатом анализа
    struct Plant {
        std::unique_ptr<Model> object;
        StabilityResult check;
        std::string label;
    };

    Plant buildPlant(int kind) {
        Plant plant;
        if (kind == 1) {
            const double a1 = readParam("a1", 0.5);
            const double a2 = readParam("a2", 0.3);
            const double b = readParam("b", 1.0);
            const int k = static_cast<int>(readParam("delay k", 2.0));
            plant.object = std::make_unique<Model1_5>(a1, a2, b, k);
            plant.check = analyzeModel1_5(a1, a2, k);
            plant.label = "Model_1_5";
        } else if (kind == 2) {
            const double a = readParam("a", 0.7);
            const double b = readParam("b", 1.0);
            plant.object = std::make_unique<Model2_9>(a, b);
            plant.check = analyzeModel2_9(a);
            plant.label = "Model_2_9";
        } else {
            const double a = readParam("a", 1.0);
            const double b = readParam("b", 1.0);
            const double dt = readParam("dt", 0.1);
            plant.object = std::make_unique<Model3_3>(a, b, dt);
            plant.check = analyzeModel3_3(a, dt);
            plant.label = "Model_3_3";
        }
        return plant;
    }

    std::unique_ptr<InputSignal> buildSignal(int kind, double amplitude) {
        if (kind == 2) {
            return std::make_unique<ImpulseSignal>(amplitude);
        }
        if (kind == 3) {
            return std::make_unique<HarmonicSignal>(amplitude);
        }
        return std::make_unique<StepSignal>(amplitude);
    }

} // namespace

int main() {
    std::puts("TMAU lab 2 (variant 15): stability check + simulation");
    std::puts("Models: 1) linear 1.5   2) nonlinear 2.9   3) Euler 3.3");

    Plant plant = buildPlant(readOption("Model number", 3));

    // ---- анализ устойчивости до начала расчёта ----
    std::printf("\nCharacteristic equation: %s\n", plant.check.equation.c_str());
    std::printf("Largest root modulus: %.5f\n", plant.check.maxModulus);
    if (!warnIfUnstable(plant.label, plant.check, std::cout)) {
        std::puts("Stable: all roots lie inside the unit circle.");
    }

    std::puts("\nSignals: 1) step   2) impulse   3) harmonic");
    const int signalKind = readOption("Signal number", 3);
    const double amplitude = readParam("Amplitude A", 1.0);
    const int count = static_cast<int>(readParam("Steps", 20.0));

    const auto signal = buildSignal(signalKind, amplitude);
    const auto trace = runSimulation(*plant.object, *signal, count);

    const std::string csvName = plant.label + "_" + signal->name() + ".csv";
    std::ofstream csv(csvName);
    csv << "t,u,y\n";

    std::printf("\n%5s %12s %14s\n", "t", "u(t)", "y(t+1)");
    for (const auto& point : trace) {
        std::printf("%5d %12.5f %14.6f\n", point.step, point.u, point.y);
        csv << point.step << ',' << point.u << ',' << point.y << '\n';
    }
    std::printf("\nSaved: %s\n", csvName.c_str());
    return 0;
}
