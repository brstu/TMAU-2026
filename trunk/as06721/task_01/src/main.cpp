#include "InputSignal.h"
#include "Models.h"

#include <cmath>
#include <clocale>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

using namespace std;

int readInt(const string& prompt, int minValue, int maxValue, int defaultValue = -1) {
    string line;
    while (true) {
        cout << prompt;
        if (defaultValue >= minValue && defaultValue <= maxValue) {
            cout << " (по умолчанию " << defaultValue << ")";
        }
        cout << ": ";

        if (!getline(cin, line)) {
            if (defaultValue >= minValue && defaultValue <= maxValue) return defaultValue;
            cin.clear();
            continue;
        }

        if (line.find_first_not_of(" \t\r\n") == string::npos) {
            if (defaultValue >= minValue && defaultValue <= maxValue) return defaultValue;
            cout << "Ошибка: введите целое число от " << minValue << " до " << maxValue << ".\n";
            continue;
        }

        istringstream input(line);
        int value;
        if (char extra; (input >> value) && !(input >> extra) && value >= minValue && value <= maxValue) {
            return value;
        }

        cout << "Ошибка: введите целое число от " << minValue << " до " << maxValue << ".\n";
    }
}

double readDouble(const string& prompt, double defaultValue) {
    string line;
    while (true) {
        cout << prompt << " (по умолчанию " << defaultValue << "): ";
        if (!getline(cin, line)) {
            return defaultValue;
        }

        if (line.find_first_not_of(" \t\r\n") == string::npos) {
            return defaultValue;
        }

        istringstream input(line);
        double value;
        // Используем C++17 init-statement внутри if
        if (char extra; (input >> value) && !(input >> extra) && isfinite(value)) {
            return value;
        }

        cout << "Ошибка: введите конечное числовое значение или нажмите Enter для значения по умолчанию.\n";
    }
}

unique_ptr<Model> chooseModel(int choice) {
    if (choice == 1) {
        cout << "\nModel 1.4: y(t+1) = a*y(t) + b*(u(t) - u(t-1))\n";
        const double a = readDouble("Введите коэффициент a", 0.8);
        const double b = readDouble("Введите коэффициент b", 1.0);
        if (abs(a) >= 1.0) {
            cout << "\nПРЕДУПРЕЖДЕНИЕ ОБ УСТОЙЧИВОСТИ: для Model 1.4 полюс равен a. "
                << "При |a| >= 1 свободная составляющая не затухает или растёт; система не является асимптотически устойчивой.\n";
        }
        else {
            cout << "Критерий устойчивости Model 1.4 выполнен: |a| < 1.\n";
        }
        return make_unique<Model14Linear>(a, b);
    }

    if (choice == 2) {
        cout << "\nModel 2.8: y(t+1) = a*y(t)*(1-y(t)) + b*u(t) + c*sin(y(t-1)*u(t))\n";
        const double a = readDouble("Введите коэффициент a", 1.0);
        const double b = readDouble("Введите коэффициент b", 0.2);
        const double c = readDouble("Введите коэффициент c", 0.1);
        return make_unique<Model28Nonlinear>(a, b, c);
    }

    cout << "\nModel 3.2: dy/dt = b*u; y(t+1) = y(t) + dt*b*u(t)\n";
    const double b = readDouble("Введите коэффициент b", 1.0);
    return make_unique<Model32Differential>(b);
}

unique_ptr<InputSignal> chooseSignal(int choice) {
    if (choice == 1) return make_unique<StepSignal>();
    if (choice == 2) return make_unique<ImpulseSignal>();
    return make_unique<HarmonicSignal>();
}

int runPlotScript() {
#ifdef _WIN32
    int result = system("py -3 plot.py");
    if (result != 0) result = system("python plot.py");
#else
    int result = system("python3 plot.py");
    if (result != 0) result = system("python plot.py");
#endif
    return result;
}

void setupWorkingDirectory(int argc, char* argv[]) {
    if (argc <= 0 || argv[0] == nullptr) return;

    try {
        const filesystem::path exePath = filesystem::absolute(argv[0]);
        if (!filesystem::exists(exePath)) return;

        filesystem::path candidate = exePath.parent_path();
        for (int level = 0; level < 4; ++level) {
            if (filesystem::exists(candidate / "plot.py")) {
                filesystem::current_path(candidate);
                return; 
            }
            if (candidate == candidate.parent_path()) {
                return; 
            }
            candidate = candidate.parent_path();
        }
    }
    catch (const filesystem::filesystem_error& e) {
        cerr << "Предупреждение: ошибка файловой системы при поиске plot.py: " << e.what() << '\n';
    }
}

int main(int argc, char* argv[]) {
#ifdef _WIN32
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
#else
    setlocale(LC_ALL, "");
#endif

    setupWorkingDirectory(argc, argv);

    cout << "  МОДЕЛИРОВАНИЕ ОБЪЕКТА УПРАВЛЕНИЯ — ВАРИАНТ 14\n";
    cout << "Доступные модели:\n";
    cout << "1. Model 1.4 — дискретный интегратор с потерями\n";
    cout << "2. Model 2.8 — логистическая модель с возмущением\n";
    cout << "3. Model 3.2 — дифференциальное уравнение dy/dt = b*u\n\n";

    const int modelChoice = readInt("Выберите модель (1-3)", 1, 3, 1);

    cout << "\nТип входного воздействия:\n";
    cout << "1. Ступенчатое: u(t) = 1\n";
    cout << "2. Импульсное: u(0) = 1, далее u(t) = 0\n";
    cout << "3. Гармоническое: u(t) = sin(t)\n";
    const int signalChoice = readInt("Выберите входное воздействие (1-3)", 1, 3, 1);

    const int n = readInt("Введите количество шагов моделирования n (1-10000)", 1, 10000, 20);
    double dt = 1.0;
    if (modelChoice == 3) {
        dt = readDouble("Введите шаг интегрирования dt (> 0)", 1.0);
        while (dt <= 0.0) {
            cout << "dt должен быть больше нуля.\n";
            dt = readDouble("Введите шаг интегрирования dt (> 0)", 1.0);
        }
    }

    unique_ptr<Model> model = chooseModel(modelChoice);
    unique_ptr<InputSignal> signal = chooseSignal(signalChoice);

    vector u(static_cast<size_t>(n), 0.0);
    vector y(static_cast<size_t>(n) + 1, 0.0);

    for (int tau = 0; tau < n; ++tau) {
        u[static_cast<size_t>(tau)] = signal->valueAt(tau);
    }

    cout << "\nВыбрана модель: " << model->name() << '\n';
    cout << "Уравнение: " << model->formula() << '\n';
    cout << "Входное воздействие: " << signal->name() << '\n';
    cout << "Начальное состояние: y(0) = 0\n\n";

    for (int tau = 0; tau < n; ++tau) {
        y[static_cast<size_t>(tau + 1)] = model->nextStep(tau, y, u, dt);
        if (!isfinite(y[static_cast<size_t>(tau + 1)])) {
            cerr << "Ошибка: результат стал бесконечным или неопределённым на шаге " << tau + 1 << ".\n";
            return 1;
        }
    }

    cout << left << setw(12) << "Шаг t+1"
        << setw(18) << "Вход u(t)"
        << setw(20) << "Выход y(t+1)" << '\n';
    cout << string(50, '-') << '\n';
    cout << fixed << setprecision(6);
    for (int tau = 0; tau < n; ++tau) {
        cout << left << setw(12) << tau + 1
            << setw(18) << u[static_cast<size_t>(tau)]
            << setw(20) << y[static_cast<size_t>(tau + 1)] << '\n';
    }

    const string csvName = "results.csv";

    string modelTitle;
    if (modelChoice == 1) {
        modelTitle = "Model 1.4";
    }
    else if (modelChoice == 2) {
        modelTitle = "Model 2.8";
    }
    else {
        modelTitle = "Model 3.2";
    }

    string signalTitle;
    if (signalChoice == 1) {
        signalTitle = "Ступенчатое";
    }
    else if (signalChoice == 2) {
        signalTitle = "Импульсное";
    }
    else {
        signalTitle = "Гармоническое";
    }

    ofstream csv(csvName);
    if (!csv.is_open()) {
        cerr << "Не удалось создать файл " << csvName << ".\n";
        return 1;
    }
    csv << "step,input_u,output_y,model_name,signal_name\n" << setprecision(12);
    for (int tau = 0; tau < n; ++tau) {
        csv << tau + 1 << ','
            << u[static_cast<size_t>(tau)] << ','
            << y[static_cast<size_t>(tau + 1)] << ','
            << modelTitle << ','
            << signalTitle << '\n';
    }
    csv.close();

    cout << "\nТаблица сохранена в файл: " << csvName << '\n';
    cout << "\nПостроение графика...\n" << flush;
    if (runPlotScript() != 0) {
        cout << "Не удалось автоматически запустить plot.py. Убедитесь, что установлен Python и matplotlib.\n";
        cout << "Установка matplotlib: py -m pip install matplotlib\n";
    }

    cout << "\nМоделирование завершено.\n";
    return 0;
}