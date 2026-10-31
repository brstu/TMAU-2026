#include <iostream>
#include <vector>
#include <iomanip>
#include <fstream>
#include <string>
#include <limits>
#include <cstdlib> // Для system()
#include "InputSignal.h"
#include "Model1_9.h"
#include "Model2_3.h"
#include "Model3_7.h"

using namespace std;

namespace {

    int readBoundedInt(const string& question, int low, int high) {
        int number;
        for (;;) {
            cout << question;
            if (cin >> number && number >= low && number <= high) return number;
            cout << "[Error] Invalid input! Please enter an integer between " << low << " and " << high << "." << endl;
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

    void runSimulation(Model& model, const string& modelTag, InputType input, const string& inputLabel, int stepCount) {
        model.reset();

        // Создаем папку doc (работает везде, даже на старых компиляторах)
        // Команда mkdir в Windows / mkdir -p в Linux/Mac. Так как у тебя Windows, используем просто mkdir.
#ifdef _WIN32
        system("if not exist doc mkdir doc");
#else
        system("mkdir -p doc");
#endif

        const string csvPath = "doc/" + modelTag + "_" + inputLabel + ".csv";
        ofstream report(csvPath);

        if (!report.is_open()) {
            cout << "[Error] Не удалось создать файл: " << csvPath << endl;
            return;
        }

        report << "Step,u,y\n";
        cout << "\n>>> Data for " << modelTag << " (" << inputLabel << " signal):\n";
        cout << setw(6) << "Step" << setw(12) << "u(t)" << setw(14) << "y(t)" << endl;

        for (int step = 0; step < stepCount; ++step) {
            const double u = getInputValue(input, step);
            const double y = model.calculateNext(u);
            report << step << "," << u << "," << y << "\n";
            cout << setw(6) << step << setw(12) << fixed << setprecision(4) << u << setw(14) << y << endl;
        }
        report.close();
    }
} // namespace

int main() {
    cout << "=== Interactive Model Configuration (Variant 19) ===" << endl;

    const int    steps = readBoundedInt("Enter number of steps (n) [1-100]: ", 1, 100);
    const double a = readReal("Enter coefficient a: ");
    const double b = readReal("Enter coefficient b: ");

    Model1_9 linModel(a * 0.4, a * 0.3, a * 0.1, b);
    Model2_3 nonlinModel(a, 0.1, b);
    Model3_7 diffModel(a, b, 0.1);

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

    switch (modelChoice) {
    case 1: runSimulation(linModel, "Linear_1_9", picked.kind, picked.label, steps); break;
    case 2: runSimulation(nonlinModel, "Nonlinear_2_3", picked.kind, picked.label, steps); break;
    case 3: runSimulation(diffModel, "DiffEquation_3_7", picked.kind, picked.label, steps); break;
    default: cout << "[Error] Unknown model choice." << endl; break;
    }

    return 0;
}