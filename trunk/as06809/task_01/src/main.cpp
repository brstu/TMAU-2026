#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <limits>
#include "InputSignal.h"
#include "Model1_6.h"
#include "Model2_10.h"
#include "Model3_4.h"

using namespace std;

int validateUserInputInt(const string& text, int min, int max) {
    int val;
    while (true) {
        cout << text;
        // Объединяем условия в один if, чтобы Satisfy SonarCloud
        if (cin >> val && val >= min && val <= max) return val;
        
        cout << "Неверный ввод. Попробуйте снова.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

double validateUserInputDouble(const string& text) {
    double val;
    while (true) {
        cout << text;
        if (cin >> val) return val;
        cout << "Неверный ввод. Попробуйте снова.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

// Добавили const & для строк, чтобы SonarCloud не ругался
void executeSimulation(Model& model, const string& fileNamePrefix, int steps, InputType sigType, const string& sigName) {
    model.reset();
    string path = "../doc/" + fileNamePrefix + "_" + sigName + ".csv";
    ofstream fout(path);
    
    fout << "Step,u,y\n";
    cout << "\n--- Модель: " << fileNamePrefix << " | Сигнал: " << sigName << " ---\n";
    
    for (int i = 0; i < steps; ++i) {
        double u = getInputValue(sigType, i);
        double y = model.calculateNext(u);
        fout << i << "," << u << "," << y << "\n";
        cout << "Шаг " << setw(3) << i << " | u = " << setw(8) << fixed << setprecision(3) << u 
             << " | y = " << setw(10) << y << "\n";
    }
    fout.close();
}

int main() {
    cout << "=== Программа моделирования (16 вариант) ===\n";
    
    int numSteps = validateUserInputInt("Введите число шагов: ", 1, 100);
    double coefA = validateUserInputDouble("Введите параметр a: ");
    double coefB = validateUserInputDouble("Введите параметр b: ");

    Model1_6 modelLin(coefA * 0.4, coefA * 0.3, coefA * 0.1, coefB); 
    Model2_10 modelNonlin(coefA, coefB, 0.1, 0.5);
    Model3_4 modelDiff(coefA, 0.1);

    int modelChoice = validateUserInputInt("\n1-Linear, 2-Nonlinear, 3-Diff. Выбор модели: ", 1, 3);
    int sigChoice = validateUserInputInt("\n1-Step, 2-Impulse, 3-Harmonic. Выбор сигнала: ", 1, 3);

    InputType selectedType;
    string selectedName;

    switch (sigChoice) {
        case 1: selectedType = InputType::STEP; selectedName = "Step"; break;
        case 2: selectedType = InputType::IMPULSE; selectedName = "Impulse"; break;
        default: selectedType = InputType::HARMONIC; selectedName = "Harmonic"; break;
    }

    if (modelChoice == 1) {
        executeSimulation(modelLin, "Linear_1_6", numSteps, selectedType, selectedName);
    } else if (modelChoice == 2) {
        executeSimulation(modelNonlin, "Nonlinear_2_10", numSteps, selectedType, selectedName);
    } else if (modelChoice == 3) {
        executeSimulation(modelDiff, "DiffEquation_3_4", numSteps, selectedType, selectedName);
    }

    return 0;
}