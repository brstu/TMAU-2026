#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "InputSignal.h"
#include "Model1_7.h"
#include "Model2_9.h"
#include "Model3_1.h"

using namespace std;

namespace {

    // ---------- Ввод с проверкой и значениями по умолчанию ----------

    string readLine(const string& question) {
        cout << question;
        string line;
        if (!getline(cin, line)) {
            // Конец ввода — дальше используются значения по умолчанию
            return "";
        }
        return line;
    }

    int readBoundedInt(const string& question, int low, int high, int defValue) {
        for (;;) {
            const string line = readLine(question + " [default " + to_string(defValue) + "]: ");
            if (line.find_first_not_of(" \t\r") == string::npos) return defValue;
            istringstream in(line);
            int number;
            if (string rest; in >> number && !(in >> rest) && number >= low && number <= high) return number;
            cout << "[Error] Please enter an integer between " << low << " and " << high << "." << endl;
            if (!cin) return defValue;
        }
    }

    double readReal(const string& question, double defValue) {
        for (;;) {
            ostringstream hint;
            hint << defValue;
            const string line = readLine(question + " [default " + hint.str() + "]: ");
            if (line.find_first_not_of(" \t\r") == string::npos) return defValue;
            istringstream in(line);
            double number;
            if (string rest; in >> number && !(in >> rest)) return number;
            cout << "[Error] Please enter a valid number." << endl;
            if (!cin) return defValue;
        }
    }

    // ---------- Описание моделей и сигналов ----------

    struct SignalChoice {
        InputType kind;
        string label;
    };

    struct ModelEntry {
        unique_ptr<Model> model;
        string tag;
    };

    const vector<SignalChoice>& allSignals() {
        static const vector<SignalChoice> signals = {
            { InputType::STEP,     "Step" },
            { InputType::IMPULSE,  "Impulse" },
            { InputType::HARMONIC, "Harmonic" }
        };
        return signals;
    }

    void ensureDocDir() {
        std::error_code ec;
        std::filesystem::create_directories("doc", ec);
    }

    // Проверка устойчивости перед запуском (дополнительное задание)
    void checkStability(const Model& model, const string& tag) {
        string info;
        const bool stable = model.isStable(info);
        cout << "\n[Stability] " << tag << ": " << info << endl;
        if (stable) {
            cout << "[Stability] OK: the process is convergent (stable)." << endl;
        } else {
            cout << "[WARNING] Coefficients lead to a DIVERGENT (unstable) process!" << endl;
        }
    }

    // Один прогон модели: таблица в консоль + CSV в doc/
    string runSimulation(Model& model, const string& modelTag,
                         const SignalChoice& signal, int stepCount) {
        model.reset();

        const string csvPath = "doc/" + modelTag + "_" + signal.label + ".csv";
        ofstream report(csvPath);
        if (!report.is_open()) {
            cout << "[Error] Cannot create file: " << csvPath << endl;
            return "";
        }

        report << "Step,u,y\n";
        cout << "\n>>> " << modelTag << " (" << signal.label << " signal)\n";
        cout << "+------+------------+--------------+\n";
        cout << "|" << setw(6) << "t" << "|" << setw(12) << "u(t)" << "|" << setw(14) << "y(t+1)" << "|\n";
        cout << "+------+------------+--------------+\n";

        for (int step = 0; step < stepCount; ++step) {
            const double u = getInputValue(signal.kind, step);
            const double y = model.calculateNext(u);
            report << step << "," << u << "," << y << "\n";
            cout << "|" << setw(6) << step
                 << "|" << setw(12) << fixed << setprecision(4) << u
                 << "|" << setw(14) << y << "|\n";
        }
        cout << "+------+------------+--------------+\n";
        cout << "Saved to " << csvPath << endl;
        return csvPath;
    }

} // namespace

int main() {
    cout << "=== Modeling of controlled objects (Variant 7) ===" << endl;
    cout << "Model 1.7 : y(t+1) = a*y(t) + b1*u(t) + b2*u(t-1) + b3*u(t-2)" << endl;
    cout << "Model 2.9 : y(t+1) = a*y(t) + b*sqrt(|u(t)|)*sign(u(t))" << endl;
    cout << "Model 3.1 : dy/dt = -a*y  (Euler method, initial value y0)" << endl;
    cout << "(press Enter to accept a default value)\n" << endl;

    const int steps = readBoundedInt("Enter number of steps n (1-1000)", 1, 1000, 30);

    cout << "\nSelect model:\n"
         << "1. Linear model 1.7\n"
         << "2. Nonlinear model 2.9\n"
         << "3. Differential equation 3.1\n"
         << "4. All models" << endl;
    const int modelChoice = readBoundedInt("Enter choice (1-4)", 1, 4, 4);

    cout << "\nSelect input signal:\n"
         << "1. Step      u = 1\n"
         << "2. Impulse   u(0) = 1, u(t>0) = 0\n"
         << "3. Harmonic  u = sin(t)\n"
         << "4. All signals" << endl;
    const int sigChoice = readBoundedInt("Enter choice (1-4)", 1, 4, 4);

    // Создание выбранных моделей (полиморфизм через указатель на Model)
    vector<ModelEntry> models;

    if (modelChoice == 1 || modelChoice == 4) {
        cout << "\n--- Parameters of Model 1.7 ---" << endl;
        const double a  = readReal("a ", 0.6);
        const double b1 = readReal("b1", 0.5);
        const double b2 = readReal("b2", 0.3);
        const double b3 = readReal("b3", 0.2);
        models.push_back({ make_unique<Model1_7>(a, b1, b2, b3), "Linear_1_7" });
    }
    if (modelChoice == 2 || modelChoice == 4) {
        cout << "\n--- Parameters of Model 2.9 ---" << endl;
        const double a = readReal("a ", 0.7);
        const double b = readReal("b ", 1.0);
        models.push_back({ make_unique<Model2_9>(a, b), "Nonlinear_2_9" });
    }
    if (modelChoice == 3 || modelChoice == 4) {
        cout << "\n--- Parameters of Model 3.1 ---" << endl;
        cout << "(input u does not enter dy/dt = -a*y, the process starts from y0)" << endl;
        const double a  = readReal("a ", 1.0);
        const double dt = readReal("dt", 0.1);
        const double y0 = readReal("y0", 1.0);
        models.push_back({ make_unique<Model3_1>(a, dt, y0), "DiffEquation_3_1" });
    }

    vector<SignalChoice> signals;
    if (sigChoice == 4) signals = allSignals();
    else signals.push_back(allSignals()[sigChoice - 1]);

    // Анализ устойчивости перед началом симуляции
    for (const auto& entry : models) checkStability(*entry.model, entry.tag);

    ensureDocDir();
    vector<string> produced;
    for (auto& entry : models) {
        for (const auto& signal : signals) {
            const string path = runSimulation(*entry.model, entry.tag, signal, steps);
            if (!path.empty()) produced.push_back(path);
        }
    }

    // Список файлов этого запуска — по нему plot.py строит графики
    ofstream list("doc/last_run.txt");
    for (const auto& path : produced) list << path << "\n";

    cout << "\nSimulation finished. Files written: " << produced.size() << endl;
    return 0;
}
