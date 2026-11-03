#include <iostream>
#include <vector>
#include <iomanip>
#include <fstream>
#include <string>
#include <limits>
#include "InputSignal.h"
#include "Model1_9.h"
#include "Model2_3.h"
#include "Model3_7.h"

using namespace std;

// Функция для безопасного ввода целых чисел (с проверкой на диапазон)
int getSafeInt(const string& prompt, int minValue, int maxValue) {
    int value;
    while (true) {
        cout << prompt;
        if (cin >> value && value >= minValue && value <= maxValue) {
            return value;
        }
        cout << "[Error] Invalid input! Please enter an integer between " 
             << minValue << " and " << maxValue << "." << endl;
        cin.clear(); // Сбрасываем флаг ошибки cin
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Очищаем буфер ввода
    }
}

// Функция для безопасного ввода дробных чисел
double getSafeDouble(const string& prompt) {
    double value;
    while (true) {
        cout << prompt;
        if (cin >> value) {
            return value;
        }
        cout << "[Error] Invalid input! Please enter a valid number." << endl;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

void runSimulation(Model& model, const string& modelName, InputType input, const string& inputName, int steps) {
    model.reset();
    string filename = "../doc/" + modelName + "_" + inputName + ".csv";
    ofstream out(filename);
    
    out << "Step,u,y\n";
    cout << "\n>>> Data for " << modelName << " (" << inputName << " signal):\n";
    cout << setw(6) << "Step" << setw(12) << "u(t)" << setw(14) << "y(t)" << endl;

    for (int t = 0; t < steps; ++t) {
        double u = getInputValue(input, t);
        double y = model.calculateNext(u);
        
        out << t << "," << u << "," << y << "\n";
        cout << setw(6) << t << setw(12) << fixed << setprecision(4) << u << setw(14) << y << endl;
    }
    out.close();
}

int main() {
    cout << "=== Interactive Model Configuration (Variant 19) ===" << endl;
    
    // Защищенный ввод параметров (Пункт 6)
    int steps = getSafeInt("Enter number of steps (n) [1-100]: ", 1, 100);
    double a  = getSafeDouble("Enter coefficient a: ");
    double b  = getSafeDouble("Enter coefficient b: ");

    // Инициализация моделей
    Model1_9 linModel(a * 0.4, a * 0.3, a * 0.1, b);
    Model2_3 nonlinModel(a, 0.1, b);
    Model3_7 diffModel(a, b, 0.1);

    // Выбор модели
    cout << "\nSelect model to calculate:" << endl;
    cout << "1. Linear Model 1.9" << endl;
    cout << "2. Nonlinear Model 2.3" << endl;
    cout << "3. Differential Equation 3.7" << endl;
    int choice = getSafeInt("Enter choice (1-3): ", 1, 3);

    // Выбор сигнала
    cout << "\nSelect input signal:" << endl;
    cout << "1. Step" << endl;
    cout << "2. Impulse" << endl;
    cout << "3. Harmonic" << endl;
    int sigChoice = getSafeInt("Enter choice (1-3): ", 1, 3);

    InputType input = InputType::STEP;
    string inputName = "Step";
    if (sigChoice == 2) { input = InputType::IMPULSE; inputName = "Impulse"; }
    if (sigChoice == 3) { input = InputType::HARMONIC; inputName = "Harmonic"; }

    // Запуск расчета строго выбранной конфигурации
    if (choice == 1) {
        runSimulation(linModel, "Linear_1_9", input, inputName, steps);
    } else if (choice == 2) {
        runSimulation(nonlinModel, "Nonlinear_2_3", input, inputName, steps);
    } else if (choice == 3) {
        runSimulation(diffModel, "DiffEquation_3_7", input, inputName, steps);
    }

    return 0;
}