#include <iostream>
#include <vector>
#include <iomanip>
#include <fstream>
#include <string>
#include <limits>
#include <cstdlib>
#include <memory>

#include "Model.h"
#include "LinearModel19.h"
#include "NonlinearModel23.h"
#include "ODE37.h"
#include "SignalGenerator.h"
#include "Simulator.h"
#include "StabilityAnalyzer.h"

using namespace std;

namespace {

int readBoundedInt(const string& question, int low, int high) {
    int number;
    for (;;) {
        cout << question;
        if (cin >> number && number >= low && number <= high) return number;
        cout << "[Error] Invalid input! Please enter an integer between "
             << low << " and " << high << "." << endl;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

double readReal(const string& question) {
    double number;
    for (;;) {
        cout << question;
        if (cin >> number) return number;
        cout << "[Error] Invalid input! Please enter a valid number." << endl;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

struct SignalChoice {
    InputType kind;
    string    label;
};

SignalChoice pickSignal(int menuIndex) {
    if (menuIndex == 2) return { InputType::IMPULSE,  "Impulse" };
    if (menuIndex == 3) return { InputType::HARMONIC, "Harmonic" };
    return { InputType::STEP, "Step" };
}

} // namespace

int main() {
    cout << "=== Interactive Model Configuration (Variant 19, Lab 2) ===" << endl;

    const int    steps = readBoundedInt("Enter number of steps (n) [1-100]: ", 1, 100);
    const double a     = readReal("Enter coefficient a: ");
    const double b     = readReal("Enter coefficient b: ");

    // Коэффициенты линейной модели 1.9 из ЛР №1 (вариант 19)
    const double a1 = a * 0.4;
    const double a2 = a * 0.3;
    const double a3 = a * 0.1;
    const double b1 = b;

    cout << "\nSelect model to calculate:" << endl;
    cout << "1. Linear Model 1.9" << endl;
    cout << "2. Nonlinear Model 2.3" << endl;
    cout << "3. Differential Equation 3.7" << endl;
    const int modelChoice = readBoundedInt("Enter choice (1-3): ", 1, 3);

    cout << "\nSelect input signal:" << endl;
    cout << "1. Step" << endl;
    cout << "2. Impulse" << endl;
    cout << "3. Harmonic" << endl;
    const int sigChoice = readBoundedInt("Enter choice (1-3): ", 1, 3);
    const SignalChoice picked = pickSignal(sigChoice);

    // Анализ устойчивости до начала симуляции (LinModel 1.9).
    StabilityAnalyzer::warnIfUnstable(a1, a2, a3);

    unique_ptr<Model> model;
    string modelTag;
    switch (modelChoice) {
    case 1:
        model = make_unique<LinearModel19>(a1, a2, a3, b1);
        modelTag = "Linear_1_9";
        break;
    case 2:
        model = make_unique<NonlinearModel23>(a, 0.1, b);
        modelTag = "Nonlinear_2_3";
        break;
    default:
        model = make_unique<ODE37>(a, b, 0.1);
        modelTag = "DiffEquation_3_7";
        break;
    }

    Simulator simulator(std::move(model), steps);
    SignalGenerator signal(picked.kind, 1.0);
    const vector<double> y = simulator.run(signal);

#ifdef _WIN32
    system("if not exist doc mkdir doc");
#else
    system("mkdir -p doc");
#endif

    const string csvPath = "doc/" + modelTag + "_" + picked.label + ".csv";
    ofstream report(csvPath);
    if (!report.is_open()) {
        cout << "[Error] Не удалось создать файл: " << csvPath << endl;
        return 1;
    }

    report << "Step,u,y\n";
    cout << "\n>>> Data for " << modelTag << " (" << picked.label << " signal):\n";
    cout << setw(6) << "Step" << setw(12) << "u(t)" << setw(14) << "y(t)" << endl;

    for (int step = 0; step < steps; ++step) {
        const double u = signal.get(step);
        report << step << "," << u << "," << y[step] << "\n";
        cout << setw(6) << step << setw(12) << fixed << setprecision(4)
             << u << setw(14) << y[step] << endl;
    }
    report.close();

    return 0;
}
