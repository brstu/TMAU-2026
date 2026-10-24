
#include <iostream>
#include <fstream>
#include <locale>
#include <iomanip>
#include <memory>
#include <string>
#include <cmath>

#ifdef _WIN32
#include <windows.h>
#endif

#include "model.h"

using namespace std;

bool readNumber(const string& prompt, double& value) {
    cout << prompt;

    if (!(cin >> value) || !isfinite(value)) {
        cout << "Ошибка: необходимо ввести конечное число.\n";
        return false;
    }

    return true;
}

unique_ptr<Model> createModel(int choice, string& modelCsvName) {
    double a;
    double b;
    double c;
    double d;
    double a1;
    double a2;
    double a3;
    double b1;
    double deltaT;

    switch (choice) {
    case 1:
        cout << "\nНастройка модели 1.9\n";

        if (!readNumber("Введите коэффициент a1: ", a1) ||
            !readNumber("Введите коэффициент a2: ", a2) ||
            !readNumber("Введите коэффициент a3: ", a3) ||
            !readNumber("Введите коэффициент b1: ", b1)) {
            return nullptr;
        }

        modelCsvName = "Model 1.9";
        return make_unique<Model1_9>(a1, a2, a3, b1);

    case 2:
        cout << "\nНастройка модели 2.1\n";

        if (!readNumber("Введите коэффициент a: ", a) ||
            !readNumber("Введите коэффициент b: ", b) ||
            !readNumber("Введите коэффициент c: ", c) ||
            !readNumber("Введите коэффициент d: ", d)) {
            return nullptr;
        }

        modelCsvName = "Model 2.1";
        return make_unique<Model2_1>(a, b, c, d);

    case 3:
        cout << "\nНастройка модели 3.3\n";

        if (!readNumber("Введите коэффициент a: ", a) ||
            !readNumber("Введите коэффициент b: ", b) ||
            !readNumber("Введите шаг времени deltaT: ", deltaT)) {
            return nullptr;
        }

        if (deltaT <= 0.0) {
            cout << "Ошибка: шаг времени должен быть больше нуля.\n";
            return nullptr;
        }

        modelCsvName = "Model 3.3";
        return make_unique<Model3_3>(a, b, deltaT);


    default:
        cout << "Ошибка: неизвестный номер модели.\n";
        return nullptr;
    }
}

bool selectSignal(InputSignals& signal, string& signalCsvName) {
    int choice;

    cout << "\nВыберите входное воздействие:\n"
         << "1 - Ступенчатое\n"
         << "2 - Импульсное\n"
         << "3 - Гармоническое\n"
         << "Ваш выбор: ";

    if (!(cin >> choice) || choice < 1 || choice > 3) {
        cout << "Ошибка: выберите вариант от 1 до 3.\n";
        return false;
    }

    signal = static_cast<InputSignals>(choice);

    switch (signal) {
    case InputSignals::Step:
        signalCsvName = "Step";
        break;
    case InputSignals::Impulse:
        signalCsvName = "Impulse";
        break;
    case InputSignals::Harmonic:
        signalCsvName = "Harmonic";
        break;
    }

    return true;
}

bool runSimulation(
    Model& model,
    int n,
    InputSignals signal,
    const string& modelCsvName,
    const string& signalCsvName
) {
    ofstream csv("result.csv", ios::out | ios::trunc);
    csv.imbue(locale::classic());

    if (!csv.is_open()) {
        cout << "Ошибка: не удалось создать result.csv.\n";
        return false;
    }

    csv << "step,u_tau,y_next,model,signal\n";
    csv << setprecision(15);

    cout << "\nМодель: " << model.getName() << "\n\n"
         << setw(13) << "Шаг"
         << setw(15) << "u(tau)"
         << setw(15) << "y(tau+1)" << '\n';
    cout << string(40, '-') << '\n';
    cout << fixed << setprecision(4);

    for (int tau = 0; tau < n; ++tau) {
        const double uTau = calculateInput(tau, signal);
        const double y = model.calculateNextStep(uTau);

        if (!isfinite(y) || fabs(y) > 1e9) {
            cout << "Ошибка: расчёт расходится на шаге "
                 << tau + 1
                 << ". Измените коэффициенты модели.\n";
            return false;
        }

        cout << setw(10) << tau + 1
             << setw(15) << uTau
             << setw(15) << y << '\n';

        csv << tau + 1 << ','
            << uTau << ','
            << y << ','
            << modelCsvName << ','
            << signalCsvName << '\n';
    }

    csv.close();

    if (!csv) {
        cout << "Ошибка при записи результатов в result.csv.\n";
        return false;
    }

    cout << "\nРезультаты сохранены в result.csv\n";
    return true;
}

int main() {
#ifdef _WIN32
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);
#endif

    int n;
    int modelChoice;

    cout << "Введите количество шагов моделирования: ";

    if (!(cin >> n) || n <= 0) {
        cout << "Ошибка: количество шагов должно быть положительным числом.\n";
        return 1;
    }

    cout << "\nВыберите модель:\n"
         << "1 - Модель 1.9 (линейная)\n"
         << "2 - Модель 2.1 (нелинейная)\n"
         << "3 - Модель 3.3 (дифференциальная)\n"
         << "Ваш выбор: ";

    if (!(cin >> modelChoice) || modelChoice < 1 || modelChoice > 3) {
        cout << "Ошибка: выберите модель от 1 до 3.\n";
        return 1;
    }

    string modelCsvName;
    auto model = createModel(modelChoice, modelCsvName);

    if (!model) {
        return 1;
    }

    InputSignals signal;
    string signalCsvName;

    if (!selectSignal(signal, signalCsvName)) {
        return 1;
    }

    return runSimulation(
        *model, n, signal, modelCsvName, signalCsvName
    ) ? 0 : 1;
}
