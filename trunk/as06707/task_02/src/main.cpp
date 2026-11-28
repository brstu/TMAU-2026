#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <cmath>
#include <iomanip>
#include <string>
#include <cstdlib> 
#include <filesystem>
#include <limits> 

// Подключаем классы из первой лабораторной 
#include "Model1_1.h"
#include "Model2_5.h"
#include "Model3_9.h"

// Подключаем новый модуль анализа устойчивости
#include "StabilityAnalyzer.h"

using namespace std;
namespace fs = std::filesystem;

int getValidInt(int minVal, int maxVal, const string& prompt) {
    int value;
    while (true) {
        cout << prompt;
        if (cin >> value && value >= minVal && value <= maxVal) {
            return value;
        } else {
            cout << "Неправильный ввод. Попробуйте еще раз." << endl;
            cin.clear(); 
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
        }
    }
}

double getValidDouble(const string& prompt) {
    double value;
    while (true) {
        cout << prompt;
        if (cin >> value) {
            return value;
        } else {
            cout << "Неправильный ввод. Введите число." << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
}

double getInput(int step, int type, double A) {
    if (type == 1) return A; 
    if (type == 2) return (step == 0) ? A : 0.0; 
    return A * sin(step * 0.1); 
}

int main(int argc, char* argv[]) {
    system("chcp 65001 > nul");

    try {
        fs::path exePath = fs::absolute(argv[0]);
        fs::path projectRoot = exePath.parent_path();
        if (projectRoot.filename() == "Release" || projectRoot.filename() == "Debug") {
            projectRoot = projectRoot.parent_path().parent_path();
        } else if (projectRoot.filename() == "build") {
            projectRoot = projectRoot.parent_path();
        }
        fs::current_path(projectRoot);
    } catch (const std::exception& e) {
        std::cerr << "Предупреждение: Не удалось изменить рабочую директорию (" << e.what() << ")" << std::endl;
    }

    cout << "Выберите модель:" << endl;
    cout << "1 - Model 1.1" << endl;
    cout << "2 - Model 2.5" << endl;
    cout << "3 - Model 3.9" << endl;
    
    int modelChoice = getValidInt(1, 3, "Ваш выбор: ");
    unique_ptr<Model> model;

    if (modelChoice == 1) {
        double a = getValidDouble("a = ");
        double b = getValidDouble("b = ");
        model = make_unique<Model1_1>(a, b);
        
        // Анализ устойчивости
        std::string warning = StabilityAnalyzer::getWarningMessage(a);
        if (!warning.empty()) {
            cout << "\n" << warning << "\n";
            cout << "Нажмите Enter для продолжения (симуляция может уйти в бесконечность)...";
            cin.ignore(); cin.get();
        }
        
    } else if (modelChoice == 2) {
        double a = getValidDouble("a = ");
        double b = getValidDouble("b = ");
        model = make_unique<Model2_5>(a, b);
    } else if (modelChoice == 3) {
        double b = getValidDouble("b = ");
        model = make_unique<Model3_9>(b);
    }

    cout << "\nТип входного воздействия:" << endl;
    cout << "1 - ступенчатое" << endl;
    cout << "2 - импульсное" << endl;
    cout << "3 - гармоническое" << endl;
    
    int signalChoice = getValidInt(1, 3, "Ваш выбор: ");

    double A = getValidDouble("Амплитуда A = ");
    int n = getValidInt(1, 1000000, "Количество шагов n = ");
    double y0 = getValidDouble("Начальное значение y0 = ");

    cout << "\nМодель: " << model->getName() << "\n\n";
    cout << setw(10) << "tau" << setw(15) << "u" << setw(15) << "y" << endl;
    cout << string(40, '-') << endl;

    string filename = "result_model" + to_string(modelChoice) + "_signal" + to_string(signalChoice) + ".csv";
    ofstream out(filename);
    out << "tau,u,y\n";

    model->reset(y0);
    double current_y = model->getY();

    for (int tau = 0; tau <= n; ++tau) {
        double u = getInput(tau, signalChoice, A);
        cout << setw(10) << tau << setw(15) << fixed << setprecision(6) << u << setw(15) << current_y << endl;
        out << tau << "," << u << "," << current_y << "\n";
        current_y = model->step(u);
    }

    out.close();
    cout << "\nРезультаты сохранены в файл: " << filename << endl;
    cout << "Запуск визуализации в Python...\n" << endl;

    #ifdef PYTHON_SCRIPT_PATH
        string scriptPath = PYTHON_SCRIPT_PATH;
    #else
        string scriptPath = "../task_01/scripts/plot_results.py"; 
    #endif

    string command = "python \"" + scriptPath + "\" " + filename + " && pause";
    
    if (int result = system(command.c_str()); result != 0) {
        cout << "\nПроизошла ошибка при вызове Python." << endl;
        system("pause");
    }

    return 0;
}