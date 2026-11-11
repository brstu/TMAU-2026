#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <locale>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;
namespace fs = std::filesystem;

#ifndef PLOT_SCRIPT_PATH
#define PLOT_SCRIPT_PATH "plot.py"
#endif

class InputSignal {
public:
    virtual ~InputSignal() = default;
    virtual double value(int tau) const = 0;
    virtual string name() const = 0;
    virtual string fileSuffix() const = 0;
};

class StepInput : public InputSignal {
private:
    double constantValue;
public:
    explicit StepInput(double value) : constantValue(value) {}
    double value(int) const override { return constantValue; }
    string name() const override { return "Ступенчатое воздействие"; }
    string fileSuffix() const override { return "step"; }
};

class ImpulseInput : public InputSignal {
public:
    double value(int tau) const override { return tau == 0 ? 1.0 : 0.0; }
    string name() const override { return "Импульсное воздействие"; }
    string fileSuffix() const override { return "impulse"; }
};

class HarmonicInput : public InputSignal {
public:
    double value(int tau) const override { return sin(static_cast<double>(tau)); }
    string name() const override { return "Гармоническое воздействие"; }
    string fileSuffix() const override { return "harmonic"; }
};

class Model {
protected:
    double previousOutput = 0.0;
    double currentOutput = 0.0;
    virtual double calculateNext(int tau, double input, double dt) = 0;
    virtual void resetInternal() {}
public:
    virtual ~Model() = default;
    virtual string name() const = 0;

    void initialize(double yMinusOne, double yZero) {
        previousOutput = yMinusOne;
        currentOutput = yZero;
        resetInternal();
    }

    double advance(int tau, double input, double dt) {
        double nextOutput = calculateNext(tau, input, dt);
        if (!isfinite(nextOutput)) {
            throw overflow_error("Получено слишком большое или неопределённое значение y.");
        }
        previousOutput = currentOutput;
        currentOutput = nextOutput;
        return currentOutput;
    }
};

// Model 1.4: y(t+1) = a*y(t) + b*(u(t) - u(t-1))
class Model14 : public Model {
private:
    double a;
    double b;
    double previousInput = 0.0;
protected:
    double calculateNext(int, double input, double) override {
        double result = a * currentOutput + b * (input - previousInput);
        previousInput = input;
        return result;
    }
    void resetInternal() override { previousInput = 0.0; }
public:
    Model14(double aValue, double bValue) : a(aValue), b(bValue) {}
    string name() const override { return "Model 1.4 — дискретный интегратор с потерями"; }
    double coefficientA() const { return a; }
};

// Model 2.6: y(t+1) = a1*y(t)^3 - a2*y(t-1) + b*u(t)^2
class Model26 : public Model {
private:
    double a1;
    double a2;
    double b;
protected:
    double calculateNext(int, double input, double) override {
        return a1 * currentOutput * currentOutput * currentOutput
            - a2 * previousOutput + b * input * input;
    }
public:
    Model26(double a1Value, double a2Value, double bValue)
        : a1(a1Value), a2(a2Value), b(bValue) {}
    string name() const override { return "Model 2.6 — асимметричная полиномиальная модель"; }
};

// Model 3.8: dy/dt = -a1*y - a2*y^2 + b*u; explicit Euler method
class Model38 : public Model {
private:
    double a1;
    double a2;
    double b;
protected:
    double calculateNext(int, double input, double dt) override {
        double derivative = -a1 * currentOutput - a2 * currentOutput * currentOutput + b * input;
        return currentOutput + dt * derivative;
    }
public:
    Model38(double a1Value, double a2Value, double bValue)
        : a1(a1Value), a2(a2Value), b(bValue) {}
    string name() const override { return "Model 3.8 — линейно-квадратичное затухание"; }
};

struct Row {
    int tau;
    double input;
    double output;
};

struct SimulationResult {
    string signalName;
    string fileSuffix;
    string csvFile;
    vector<Row> rows;
};

string readLine(const string& prompt) {
    cout << prompt;
    string line;
    if (!getline(cin, line)) {
        throw runtime_error("Ввод был неожиданно завершён.");
    }
    return line;
}

int readIntDefault(const string& prompt, int minValue, int maxValue, int defaultValue) {
    while (true) {
        string line = readLine(prompt + " [" + to_string(defaultValue) + "]: ");
        if (line.empty()) return defaultValue;

        istringstream input(line);
        int value;
        char extra;
        if ((input >> value) && !(input >> extra) && value >= minValue && value <= maxValue) {
            return value;
        }
        cout << "Ошибка: введите целое число от " << minValue << " до " << maxValue
             << " или нажмите Enter для значения по умолчанию.\n";
    }
}

double readDoubleDefault(const string& prompt, double defaultValue) {
    while (true) {
        string line = readLine(prompt + " [" + to_string(defaultValue) + "]: ");
        if (line.empty()) return defaultValue;

        for (char& ch : line) {
            if (ch == ',') ch = '.';
        }

        istringstream input(line);
        input.imbue(locale::classic());
        double value;
        char extra;
        if ((input >> value) && !(input >> extra) && isfinite(value)) {
            return value;
        }
        cout << "Ошибка: введите конечное число (десятичный разделитель — точка или запятая).\n";
    }
}

void printTable(const vector<Row>& rows, const string& modelName, const string& signalName) {
    cout << "\n" << modelName << "\n" << signalName << "\n";
    cout << "------------------------------------------------------\n";
    cout << left << setw(10) << "tau" << setw(20) << "u_tau" << "y_tau\n";
    cout << "------------------------------------------------------\n";
    cout << fixed << setprecision(6);
    for (const Row& row : rows) {
        cout << left << setw(10) << row.tau
             << setw(20) << row.input
             << row.output << '\n';
    }
    cout << "------------------------------------------------------\n";
}

void saveCsv(const vector<Row>& rows, const string& fileName) {
    ofstream file(fileName);
    if (!file.is_open()) {
        throw runtime_error("Не удалось создать файл " + fileName);
    }
    file.imbue(locale::classic());
    file << "tau;u_tau;y_tau\n";
    file << setprecision(12);
    for (const Row& row : rows) {
        file << row.tau << ';' << row.input << ';' << row.output << '\n';
    }
}

vector<Row> simulate(Model& model, const InputSignal& signal, int n, double dt,
                     double yMinusOne, double yZero) {
    model.initialize(yMinusOne, yZero);
    vector<Row> rows;
    rows.reserve(static_cast<size_t>(n) + 1);
    rows.push_back({0, signal.value(0), yZero});

    for (int tau = 0; tau < n; ++tau) {
        double input = signal.value(tau);
        double nextOutput = model.advance(tau, input, dt);
        rows.push_back({tau + 1, signal.value(tau + 1), nextOutput});
    }
    return rows;
}

void clearOldResults() {
    for (const char* suffix : {"step", "impulse", "harmonic"}) {
        error_code ec;
        fs::remove(string("results_") + suffix + ".csv", ec);
        fs::remove(string("plot_data_") + suffix + ".csv", ec);
    }
}

unique_ptr<Model> createModel(int modelChoice, double p1, double p2, double p3) {
    if (modelChoice == 1) return make_unique<Model14>(p1, p2);
    if (modelChoice == 2) return make_unique<Model26>(p1, p2, p3);
    return make_unique<Model38>(p1, p2, p3);
}

string quoteArgument(const string& value) {
    string result = "\"";
    for (char ch : value) {
        if (ch == '"') result += '\\';
        result += ch;
    }
    result += '"';
    return result;
}

int runPlotScript(const string& modelId, const vector<string>& csvFiles) {
    fs::path scriptPath(PLOT_SCRIPT_PATH);
    if (!fs::exists(scriptPath)) {
        cerr << "Не найден файл построения графиков: " << scriptPath.string() << '\n';
        return -1;
    }

    string arguments = quoteArgument(scriptPath.string()) + " " + modelId;
    for (const string& fileName : csvFiles) {
        arguments += " " + quoteArgument(fileName);
    }

#ifdef _WIN32
    const vector<string> interpreters = {"py", "python", "python3"};
#else
    const vector<string> interpreters = {"python3", "python"};
#endif

    for (const string& interpreter : interpreters) {
        string command = interpreter + " " + arguments;
        cout << "\nЗапуск построения графиков: " << interpreter << " ...\n";
        cout.flush();
        int result = system(command.c_str());
        if (result == 0) return 0;
    }

    cerr << "Не удалось запустить plot.py. Проверьте установку Python и Matplotlib.\n";
    cerr << "Команда установки Matplotlib: py -m pip install matplotlib\n";
    return -1;
}

int main() {
#ifdef _WIN32
    system("chcp 65001 > nul");
#endif
    cout << "======================================================\n";
    cout << "       МОДЕЛИРОВАНИЕ ОБЪЕКТА УПРАВЛЕНИЯ\n";
    cout << "                   ВАРИАНТ 4\n";
    cout << "======================================================\n";

    try {
        cout << "\nВыберите модель:\n";
        cout << "1. Model 1.4 — линейная\n";
        cout << "2. Model 2.6 — нелинейная\n";
        cout << "3. Model 3.8 — дифференциальное уравнение\n";
        int modelChoice = readIntDefault("Ваш выбор", 1, 3, 1);

        string modelTitle;
        if (modelChoice == 1) modelTitle = "Model 1.4 — дискретный интегратор с потерями";
        else if (modelChoice == 2) modelTitle = "Model 2.6 — асимметричная полиномиальная модель";
        else modelTitle = "Model 3.8 — линейно-квадратичное затухание";

        int n = readIntDefault("Количество шагов симуляции n (1..10000)", 1, 10000, 20);

        double p1 = 0.0;
        double p2 = 0.0;
        double p3 = 0.0;
        double yMinusOne = 0.0;
        double yZero = 0.0;
        double dt = 1.0;

        if (modelChoice == 1) {
            cout << "\nПараметры Model 1.4: y(t+1) = a*y(t) + b*(u(t)-u(t-1))\n";
            p1 = readDoubleDefault("Коэффициент a", 0.5);
            p2 = readDoubleDefault("Коэффициент b", 1.0);
            yZero = readDoubleDefault("Начальное значение y(0)", 0.0);
        } else if (modelChoice == 2) {
            cout << "\nПараметры Model 2.6: y(t+1) = a1*y(t)^3 - a2*y(t-1) + b*u(t)^2\n";
            p1 = readDoubleDefault("Коэффициент a1", 0.1);
            p2 = readDoubleDefault("Коэффициент a2", 0.2);
            p3 = readDoubleDefault("Коэффициент b", 0.5);
            yMinusOne = readDoubleDefault("Начальное значение y(-1)", 0.0);
            yZero = readDoubleDefault("Начальное значение y(0)", 0.1);
        } else {
            cout << "\nПараметры Model 3.8: dy/dt = -a1*y - a2*y^2 + b*u\n";
            p1 = readDoubleDefault("Коэффициент a1", 0.5);
            p2 = readDoubleDefault("Коэффициент a2", 0.1);
            p3 = readDoubleDefault("Коэффициент b", 1.0);
            dt = readDoubleDefault("Шаг интегрирования dt (больше 0)", 0.1);
            while (dt <= 0.0) {
                cout << "Шаг dt должен быть больше нуля.\n";
                dt = readDoubleDefault("Введите dt", 0.1);
            }
            yZero = readDoubleDefault("Начальное значение y(0)", 0.0);
        }

        cout << "\nВыберите входное воздействие:\n";
        cout << "1. Ступенчатое: u(t) = C\n";
        cout << "2. Импульсное: u(0) = 1, u(t > 0) = 0\n";
        cout << "3. Гармоническое: u(t) = sin(t)\n";
        int signalChoice = readIntDefault("Ваш выбор", 1, 3, 1);

        clearOldResults();

        vector<unique_ptr<InputSignal>> signals;
        if (signalChoice == 1) {
            double constantValue = readDoubleDefault(
                "Постоянное значение C для ступенчатого воздействия", 1.0);
            signals.push_back(make_unique<StepInput>(constantValue));
        } else if (signalChoice == 2) {
            signals.push_back(make_unique<ImpulseInput>());
        } else {
            signals.push_back(make_unique<HarmonicInput>());
        }

        vector<SimulationResult> results;
        for (const auto& signal : signals) {
            unique_ptr<Model> model = createModel(modelChoice, p1, p2, p3);
            SimulationResult result;
            result.signalName = signal->name();
            result.fileSuffix = signal->fileSuffix();
            result.rows = simulate(*model, *signal, n, dt, yMinusOne, yZero);
            results.push_back(result);
        }

        vector<string> plotFiles;

        for (SimulationResult& result : results) {
            const string fileName = "results_" + result.fileSuffix + ".csv";
            saveCsv(result.rows, fileName);
            result.csvFile = fileName;
            plotFiles.push_back(fileName);

            printTable(result.rows, modelTitle, result.signalName);
            cout << "Результаты сохранены в файл: " << fileName << "\n";
        }

        string modelId;
        if (modelChoice == 1) modelId = "Model_1_4";
        else if (modelChoice == 2) modelId = "Model_2_6";
        else modelId = "Model_3_8";

        cout << "\nМоделирование завершено. Передача результатов в plot.py...\n";
        cout.flush();
        int plotResult = runPlotScript(modelId, plotFiles);

        if (plotResult == 0) {
            cout << "График построен и сохранён в results_plot.png.\n";
        } else {
            cout << "Расчёт завершён, но график автоматически построить не удалось.\n";
        }
    } catch (const exception& error) {
        cerr << "\nОшибка: " << error.what() << '\n';
        return 1;
    }

    cout << "\nРабота программы завершена.\n";
    return 0;
}
