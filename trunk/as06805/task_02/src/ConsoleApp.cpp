#include "ConsoleApp.h"

#include <fstream>
#include <iomanip>
#include <istream>
#include <limits>
#include <memory>
#include <ostream>
#include <sstream>
#include <utility>
#include <vector>

#include "LinearModel11.h"
#include "LinearModel13.h"
#include "Model.h"
#include "NonlinearModel22.h"
#include "NonlinearModel23.h"
#include "NonlinearModel25.h"
#include "ODEModel37.h"
#include "SignalGenerator.h"
#include "Simulator.h"
#include "StabilityAnalyzer.h"

namespace {

// Парсит строку в double (допускается запятая как разделитель дробной части).
bool tryParseDouble(std::string input, double& outValue) {
    for (char& c : input) {
        if (c == ',') {
            c = '.';
        }
    }
    std::stringstream ss(input);
    return (ss >> outValue) && ss.eof();
}

double getValidDouble(std::istream& in, std::ostream& out, const std::string& prompt) {
    double value = 0.0;
    std::string input;
    while (true) {
        out << prompt;
        if (in >> input && tryParseDouble(input, value)) {
            return value;
        }
        out << " Ошибка: введите корректное число (например, 0.8 или -0.2).\n";
        in.clear();
        in.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
    }
}

int getValidInt(std::istream& in, std::ostream& out, const std::string& prompt,
                int minVal = 1, int maxVal = 10000) {
    int value = 0;
    while (true) {
        out << prompt;
        if (in >> value && value >= minVal && value <= maxVal) {
            return value;
        }
        out << " Ошибка: введите целое число от " << minVal << " до " << maxVal << ".\n";
        in.clear();
        in.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
    }
}

bool confirm(std::istream& in, std::ostream& out, const std::string& prompt) {
    out << prompt;
    std::string answer;
    if (!(in >> answer)) {
        return false;
    }
    return answer == "1" || answer == "y" || answer == "Y" || answer == "д" || answer == "Д";
}

void printMenu(std::ostream& out) {
    out << "=== МОДЕЛИРОВАНИЕ ДИНАМИЧЕСКИХ СИСТЕМ (задание 2) ===\n\n";
    out << "Выберите модель для исследования:\n";
    out << "1. LinearModel11  (Linear 1st order)\n";
    out << "2. LinearModel13  (Linear 2nd order)\n";
    out << "3. NonlinearModel22 (Actuator saturation)\n";
    out << "4. NonlinearModel23 (Dead-zone non-linearity)\n";
    out << "5. NonlinearModel25 (Nonlinear friction)\n";
    out << "6. ODEModel37 (ODE Euler)\n";
}

void printSignalMenu(std::ostream& out) {
    out << "\nВыберите тип входного сигнала:\n";
    out << "1. Ступенчатый (STEP)\n";
    out << "2. Импульсный (IMPULSE)\n";
    out << "3. Гармонический (HARMONIC)\n";
}

std::unique_ptr<Model> createModel(int modelChoice, std::istream& in, std::ostream& out) {
    switch (modelChoice) {
        case 1: {
            out << "\n--- Параметры LinearModel11 ---\n";
            const double a = getValidDouble(in, out, "Введите a [например, 0.8]: ");
            const double b = getValidDouble(in, out, "Введите b [например, 0.5]: ");
            return std::make_unique<LinearModel11>(a, b);
        }
        case 2: {
            out << "\n--- Параметры LinearModel13 ---\n";
            const double a1 = getValidDouble(in, out, "Введите a1 [например, 0.5]: ");
            const double a2 = getValidDouble(in, out, "Введите a2 [например, 0.1]: ");
            const double b = getValidDouble(in, out, "Введите b [например, 1.0]: ");
            return std::make_unique<LinearModel13>(a1, a2, b);
        }
        case 3: {
            out << "\n--- Параметры NonlinearModel22 ---\n";
            const double a = getValidDouble(in, out, "Введите a [например, 0.7]: ");
            const double b = getValidDouble(in, out, "Введите b [например, 0.3]: ");
            const double uMax = getValidDouble(in, out, "Введите Umax [например, 1.0]: ");
            const double uMin = getValidDouble(in, out, "Введите Umin [например, -1.0]: ");
            return std::make_unique<NonlinearModel22>(a, b, uMin, uMax);
        }
        case 4: {
            out << "\n--- Параметры NonlinearModel23 ---\n";
            const double a = getValidDouble(in, out, "Введите a [например, 0.7]: ");
            const double b = getValidDouble(in, out, "Введите b [например, 0.3]: ");
            const double delta = getValidDouble(in, out, "Введите delta (зона нечувствительности) [например, 0.1]: ");
            return std::make_unique<NonlinearModel23>(a, b, delta);
        }
        case 5: {
            out << "\n--- Параметры NonlinearModel25 ---\n";
            const double a = getValidDouble(in, out, "Введите a [например, 0.8]: ");
            const double b = getValidDouble(in, out, "Введите b [например, 0.5]: ");
            return std::make_unique<NonlinearModel25>(a, b);
        }
        default: {
            out << "\n--- Параметры ODEModel37 ---\n";
            const double a = getValidDouble(in, out, "Введите a [например, 0.1]: ");
            const double b = getValidDouble(in, out, "Введите b [например, 1.0]: ");
            const double dt = getValidDouble(in, out, "Введите шаг интегрирования dt [например, 0.1]: ");
            return std::make_unique<ODEModel37>(a, b, dt);
        }
    }
}

void exportResultsTxt(const std::string& fileName, const std::string& modelName,
                       const std::string& signalName, const SignalGenerator& signal,
                       const std::vector<double>& results) {
    if (fileName.empty()) {
        return;
    }
    std::ofstream fout(fileName);
    if (!fout.is_open()) {
        return;
    }
    fout << "# " << modelName << "|" << signalName << "\n";
    for (size_t i = 0; i < results.size(); ++i) {
        fout << i << " " << signal.get(static_cast<double>(i)) << " " << results[i] << "\n";
    }
}

}  // namespace

int runApplication(std::istream& in, std::ostream& out, const AppOptions& options) {
    printMenu(out);

    const int modelChoice = getValidInt(in, out, "Ваш выбор модели (1-6) [например, 2]: ", 1, 6);

    printSignalMenu(out);

    const int signalChoice = getValidInt(in, out, "Ваш выбор сигнала (1-3) [например, 1]: ", 1, 3);
    const double amplitude = getValidDouble(in, out, "Введите амплитуду сигнала [например, 1.0]: ");

    SignalType sigType = SignalType::STEP;
    if (signalChoice == 2) {
        sigType = SignalType::IMPULSE;
    } else if (signalChoice == 3) {
        sigType = SignalType::HARMONIC;
    }
    const std::string signalName = SignalGenerator::typeName(sigType);

    SignalGenerator signalGen(sigType, amplitude);

    const int steps = getValidInt(in, out, "\nВведите количество шагов N (1..10000) [например, 50]: ", 1, 10000);

    std::unique_ptr<Model> model = createModel(modelChoice, in, out);
    const std::string modelName = model->name();

    // --- Анализ устойчивости: проверка полюсов на Z-плоскости ---
    out << "\n" << StabilityAnalyzer::formatReport(*model);

    if (StabilityAnalyzer::warnIfUnstable(*model, out)) {
        if (!confirm(in, out, "Продолжить симуляцию неустойчивого процесса? (1 — да, 0 — нет): ")) {
            out << "\nСимуляция прервана пользователем.\n";
            return 0;
        }
    }

    Simulator simulator(std::move(model), steps);
    const std::vector<double> results = simulator.run(signalGen);

    simulator.printTable(results, signalGen, out);

    if (!options.csvFileName.empty()) {
        simulator.exportCSV(results, signalGen, options.csvFileName);
    }
    exportResultsTxt(options.txtFileName, modelName, signalName, signalGen, results);

    out << "\n[Инфо] Результаты сохранены в " << options.csvFileName << " и "
        << options.txtFileName << "\n";

    return 0;
}
