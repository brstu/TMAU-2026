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

#include "Model1_1.h"
#include "Model2_5.h"
#include "Model3_9.h"

using namespace std;
namespace fs = std::filesystem;

// Функция для безопасного ввода целых чисел в заданном диапазоне
int getValidInt(int minVal, int maxVal, const string& prompt) {
    int value;
    while (true) {
        cout << prompt;
        if (cin >> value && value >= minVal && value <= maxVal) {
            return value;
        } else {
            cout << "Неправильный ввод. Попробуйте еще раз." << endl;
            cin.clear(); // Сбрасываем флаг ошибки
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Очищаем буфер от мусора
        }
    }
}

// Функция для безопасного ввода дробных чисел (чтобы не падало при вводе букв)
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

// Функция генерации входного воздействия
double getInput(int step, int type, double A) {
    if (type == 1) return A; 
    if (type == 2) return (step == 0) ? A : 0.0; 
    return A * sin(step); 
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
    
    // Используем защищенный ввод
    int modelChoice = getValidInt(1, 3, "Ваш выбор: ");

    unique_ptr<Model> model;

    if (modelChoice == 1) {
        double a = getValidDouble("a = ");
        double b = getValidDouble("b = ");
        model = make_unique<Model1_1>(a, b);
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
    
    // Защищенный ввод для типа сигнала
    int signalChoice = getValidInt(1, 3, "Ваш выбор: ");

    double A = getValidDouble("Амплитуда A = ");
    int n = getValidInt(1, 1000000, "Количество шагов n = "); // Ограничим разумными рамками
    double y0 = getValidDouble("Начальное значение y0 = ");

    cout << "\nМодель: " << model->getName() << "\n\n";
    cout << setw(10) << "tau" << setw(15) << "u" << setw(15) << "y" << endl;
    cout << string(40, '-') << endl;

    string filename = "result_model" + to_string(modelChoice) + "_signal" + to_string(signalChoice) + ".csv";
    ofstream out(filename);
    out << "tau,u,y\n";

    if (modelChoice == 1 && !dynamic_cast<Model1_1*>(model.get())->isStable()) {
        cout << "Модель неустойчива (|a| >= 1)!" << endl;
    }

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

    string command = "python scripts/plot_results.py " + filename + " && pause";
    
    if (int result = system(command.c_str()); result != 0) {
        cout << "\nПроизошла ошибка при вызове Python." << endl;
        system("pause");
    }

    return 0;
}