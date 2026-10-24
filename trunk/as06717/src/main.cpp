#include <iostream>
#include <vector>
#include <fstream>
#include <iomanip>
#include <memory>
#include <string>
#include "input_signal.h"
#include "model1-8.h"
#include "model2-10.h"
#include "model3-2.h"

#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;

static int readChoice(const string& prompt) {
    int val = 0;
    cout << prompt;
    cin >> val;
    return val;
}

static unique_ptr<Model> pickModel() {
    cout << "\nAvailable models:\n"
         << "  1 - AR Linear Model\n"
         << "  2 - Tanh Smoothing Model\n"
         << "  3 - Integrator Model\n";

    int id = readChoice("Your choice: ");

    if (id == 1) {
        double a1, a2, b1, b2;
        cout << "\nCoefficients:\n";
        cout << "  b1 = "; cin >> b1;
        cout << "  b2 = "; cin >> b2;
        cout << "  a1 = "; cin >> a1;
        cout << "  a2 = "; cin >> a2;
        return make_unique<ArModel>(a1, a2, b1, b2);
    }
    if (id == 2) {
        double alpha, beta;
        cout << "\nCoefficients:\n";
        cout << "  alpha = "; cin >> alpha;
        cout << "  beta  = "; cin >> beta;
        return make_unique<TanhModel>(alpha, beta);
    }
    double beta;
    cout << "\nCoefficient:\n";
    cout << "  beta = "; cin >> beta;
    return make_unique<IntegratorModel>(beta);
}

static InputType pickSignal() {
    cout << "\nSignal type:\n"
         << "  1 - step\n"
         << "  2 - impulse\n"
         << "  3 - harmonic\n";

    int id = readChoice("Your choice: ");
    if (id == 2) return InputType::Impulse;
    if (id == 3) return InputType::Harmonic;
    return InputType::Step;
}

static string signalName(InputType t) {
    if (t == InputType::Step)     return "Ступенчатое";
    if (t == InputType::Impulse)  return "Импульсное";
    return "Гармоническое";
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
#endif

    cout << "=========================================\n"
         << " Lab 1 #8                                \n"
         << "=========================================\n";

    auto sim = pickModel();
    InputType sig_type = pickSignal();

    double sig_amp = 1.0;
    int    n_steps = 10;
    double y0      = 0.0;

    cout << "\nSimulation settings:\n";
    cout << "  amplitude A   = "; cin >> sig_amp;
    cout << "  iterations n  = "; cin >> n_steps;
    cout << "  initial y0    = "; cin >> y0;

    sim->setInitial(y0);
    cout << "\nSelected model: " << sim->getName() << "\n\n";

    auto row = [](auto col1, auto col2, auto col3) {
        cout << left
             << setw(10) << col1
             << setw(14) << col2
             << setw(14) << col3 << "\n";
    };

    row("step", "input", "output");
    cout << "------------------------------------\n";

    vector<int>    time_axis;
    vector<double> input_arr;
    vector<double> output_arr;

    double prev_in_a = 0.0;
    double prev_in_b = 0.0;

    for (int step = 1; step <= n_steps; ++step) {
        double in_now  = InputSignal::getSignal(sig_type, step, sig_amp);
        double out_now = sim->calculateStep(in_now, prev_in_a, prev_in_b);

        prev_in_b = prev_in_a;
        prev_in_a = in_now;

        time_axis.push_back(step);
        input_arr.push_back(in_now);
        output_arr.push_back(out_now);

        row(step, in_now, out_now);
    }

    ofstream csv_out("doc/results.csv");
    if (!csv_out.is_open()) {
        cout << "\n[error] cannot open doc/results.csv — create the 'doc' folder!\n";
        return 1;
    }

    csv_out << sim->getName() << ";" << signalName(sig_type) << "\n";

    for (size_t k = 0; k < time_axis.size(); ++k) {
        csv_out << time_axis.at(k) << ";"
                << input_arr.at(k)  << ";"
                << output_arr.at(k) << "\n";
    }
    csv_out.close();

    cout << "\nResults saved to doc/results.csv\n"
         << "Plot generation started...\n";
    return 0;
}