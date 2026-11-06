#include <iostream>
#include <vector>
#include <iomanip>
#include <cmath>
#include <fstream>
#include <algorithm>
#include <string>
#include <windows.h>

// Подключаем заголовочные файлы моделей
#include "LinearModel.h"
#include "NonLinearModel.h"
#include "DiffEqModel.h"

// Функция для генерации входного сигнала u(t)
double getInputSignal(int type, double t) {
    if (type == 1) return 1.0; // Ступенька
    if (type == 2) return (t < 0.01) ? 1.0 : 0.0; // Импульс
    if (type == 3) return sin(t); // Синус
    return 0.0;
}

void runSimulation(Model* model, const std::string& modelName, int signalType, int steps, double dt) {
    std::cout << "\n--- Simulating " << modelName << " | Signal: " << signalType << " ---\n";
    std::cout << std::setw(10) << "Step" << std::setw(15) << "Time" << std::setw(15) << "Input U" << std::setw(15) << "Output Y" << std::endl;

    // Безопасное имя файла
    std::string safeName = modelName;
    std::replace(safeName.begin(), safeName.end(), ' ', '_');
    std::replace(safeName.begin(), safeName.end(), '.', '_');
    std::string filename = safeName + "_signal" + std::to_string(signalType) + ".csv";
    
    std::ofstream outFile(filename);
    outFile << "Time,Input,Output\n";

    for (int i = 0; i < steps; ++i) {
        double time = i * dt;
        double u = getInputSignal(signalType, time);
        
        model->step(u);
        double y = model->getY();

        std::cout << std::setw(10) << i 
                  << std::setw(15) << std::fixed << std::setprecision(3) << time
                  << std::setw(15) << u
                  << std::setw(15) << y << std::endl;

        outFile << time << "," << u << "," << y << "\n";
    }
    outFile.close();
    std::cout << "Data saved to " << filename << std::endl;
}

int main() {
    // Настройка кодировки консоли
    SetConsoleCP(1251);       
    SetConsoleOutputCP(1251); 

    int choiceModel, choiceSignal, steps;
    double y0, dt;
    double a, b, a1, a2;

    std::cout << "=== LAB 1: VARIANT 4 ===\n";
    
    // 1. Выбор модели
    std::cout << "Select model:\n";
    std::cout << "1 - Linear Model 1.4\n";
    std::cout << "2 - NonLinear Model 2.6\n";
    std::cout << "3 - DiffEq Model 3.8\n";
    std::cout << "Enter choice (1-3): ";
    std::cin >> choiceModel;

    // 2. Ввод коэффициентов в зависимости от выбранной модели
    if (choiceModel == 1) {
        std::cout << "\nEnter coefficients for Linear Model 1.4:\n";
        std::cout << "Enter a: "; std::cin >> a;
        std::cout << "Enter b: "; std::cin >> b;
        
        if (std::abs(a) >= 1.0) {
            std::cout << "\n[WARNING] Coefficient 'a' >= 1. System is UNSTABLE!\n";
        }
    } 
    else if (choiceModel == 2) {
        std::cout << "\nEnter coefficients for NonLinear Model 2.6:\n";
        std::cout << "Enter a1: "; std::cin >> a1;
        std::cout << "Enter a2: "; std::cin >> a2;
        std::cout << "Enter b: "; std::cin >> b;
    } 
    else if (choiceModel == 3) {
        std::cout << "\nEnter coefficients for DiffEq Model 3.8:\n";
        std::cout << "Enter a1: "; std::cin >> a1;
        std::cout << "Enter a2: "; std::cin >> a2;
        std::cout << "Enter b: "; std::cin >> b;
    } 
    else {
        std::cout << "Invalid model choice!\n";
        return 1;
    }

    // 3. Выбор сигнала
    std::cout << "\nSelect input signal:\n";
    std::cout << "1 - Step\n";
    std::cout << "2 - Impulse\n";
    std::cout << "3 - Harmonic\n";
    std::cout << "Enter choice (1-3): ";
    std::cin >> choiceSignal;

    // 4. Ввод количества шагов
    std::cout << "\nEnter number of steps (n): ";
    std::cin >> steps;

    // 5. Ввод дополнительных параметров
    std::cout << "Enter initial value Y(0) [e.g., 0.0]: ";
    std::cin >> y0;
    std::cout << "Enter time step dt [e.g., 0.1]: ";
    std::cin >> dt;

    // Создаем нужную модель на основе выбора
    Model* currentModel = nullptr;
    std::string modelName;

    if (choiceModel == 1) {
        currentModel = new LinearModel(a, b, y0, dt);
        modelName = "Linear_1_4";
    } else if (choiceModel == 2) {
        currentModel = new NonLinearModel(a1, a2, b, y0, dt);
        modelName = "NonLinear_2_6";
    } else if (choiceModel == 3) {
        currentModel = new DiffEqModel(a1, a2, b, y0, dt);
        modelName = "DiffEq_3_8";
    }

    // Запуск симуляции
    runSimulation(currentModel, modelName, choiceSignal, steps, dt);

    delete currentModel;
    return 0;
}   