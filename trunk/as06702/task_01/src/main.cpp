#include <cmath>
#include <complex>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#else
#include <unistd.h>
#endif

using namespace std;

constexpr double PI = 3.14159265358979323846;

#ifndef PROJECT_ROOT
#define PROJECT_ROOT "."
#endif


enum class InputType {
    Step,
    Impulse,
    Harmonic
};

string inputName(InputType type) {
    switch (type) {
    case InputType::Step: return "Step";
    case InputType::Impulse: return "Impulse";
    case InputType::Harmonic: return "Harmonic";
    }
    return "Unknown";
}

double inputValue(InputType type, int tau) {
    switch (type) {
    case InputType::Step:
        return 1.0;
    case InputType::Impulse:
        return tau == 0 ? 1.0 : 0.0;
    case InputType::Harmonic:
        return sin(static_cast<double>(tau));
    }
    return 0.0;
}

class Model {
protected:
    double y_;
    double initialY_;

public:
    explicit Model(double initialY) : y_(initialY), initialY_(initialY) {}
    virtual ~Model() = default;

    virtual double step(double u, double dt) = 0;
    virtual string name() const = 0;
    virtual void reset() { y_ = initialY_; }
    double value() const { return y_; }
};

class LinearModel13 final : public Model {
private:
    double a1_;
    double a2_;
    double b_;
    double previousY_;

public:
    LinearModel13(double a1, double a2, double b)
        : Model(0.0), a1_(a1), a2_(a2), b_(b), previousY_(0.0) {
    }

    double step(double u, double /*dt*/) override {
        const double nextY = a1_ * y_ + a2_ * previousY_ + b_ * u;
        previousY_ = y_;
        y_ = nextY;
        return y_;
    }

    string name() const override { return "Model 1.3 - Linear"; }

    void reset() override {
        Model::reset();
        previousY_ = 0.0;
    }

    double a1() const { return a1_; }
    double a2() const { return a2_; }
};

class RelayModel27 final : public Model {
private:
    double a_;
    double b_;
    double epsilon_;

    double relay(double u) const {
        if (u > epsilon_) return 1.0;
        if (u < -epsilon_) return -1.0;
        return y_ > 0.0 ? 1.0 : -1.0;
    }

public:
    RelayModel27(double a, double b, double epsilon)
        : Model(0.0), a_(a), b_(b), epsilon_(epsilon) {
    }

    double step(double u, double /*dt*/) override {
        y_ = a_ * y_ + b_ * relay(u);
        return y_;
    }

    string name() const override { return "Model 2.7 - Relay"; }
};

class DifferentialModel31 final : public Model {
private:
    double a_;

public:
    explicit DifferentialModel31(double a) : Model(1.0), a_(a) {}

    double step(double /*u*/, double dt) override {
        const double derivative = -a_ * y_;
        y_ += dt * derivative;
        return y_;
    }

    string name() const override { return "Model 3.1 - Differential"; }
};

struct ResultRow {
    string model;
    string input;
    int tau;
    double u;
    double y;
};

class Simulator {
public:
    static vector<ResultRow> run(Model& model, InputType inputType, int n, double dt) {
        vector<ResultRow> rows;
        rows.reserve(static_cast<size_t>(n));
        model.reset();

        for (int tau = 0; tau < n; ++tau) {
            const double u = inputValue(inputType, tau);
            const double y = model.step(u, dt);
            rows.push_back({ model.name(), inputName(inputType), tau + 1, u, y });
        }
        return rows;
    }
};

class ResultWriter {
public:
    static void printTable(const vector<ResultRow>& rows) {
        cout << "\nРезультаты моделирования (время tau начинается с 1):\n";
        cout << left << setw(29) << "Модель"
            << setw(12) << "Воздействие"
            << right << setw(7) << "tau"
            << setw(15) << "u_tau"
            << setw(17) << "y_tau" << '\n';
        cout << string(80, '-') << '\n';
        cout << fixed << setprecision(6);
        for (const auto& row : rows) {
            cout << left << setw(29) << row.model
                << setw(12) << row.input
                << right << setw(7) << row.tau
                << setw(15) << row.u
                << setw(17) << row.y << '\n';
        }
    }

    static bool saveCsv(const vector<ResultRow>& rows, const string& fileName) {
        ofstream file(fileName);
        if (!file) return false;
        file << "model,input,tau,u,y\n";
        file << setprecision(12);
        for (const auto& row : rows) {
            file << '"' << row.model << "\",\"" << row.input << "\"," << row.tau
                << ',' << row.u << ',' << row.y << '\n';
        }
        return true;
    }
};

int readInt(const string& prompt, int defaultValue, int minValue, int maxValue) {
    while (true) {
        cout << prompt << " [по умолчанию " << defaultValue << "]: ";
        string line;
        getline(cin, line);
        if (line.empty()) return defaultValue;

        try {
            size_t pos = 0;
            int value = stoi(line, &pos);
            if (pos == line.size() && value >= minValue && value <= maxValue) return value;
        }
        catch (const exception&) {
        }
        cout << "Введите целое число от " << minValue << " до " << maxValue << ".\n";
    }
}

double readDouble(const string& prompt, double defaultValue) {
    while (true) {
        cout << prompt << " [по умолчанию " << defaultValue << "]: ";
        string line;
        getline(cin, line);
        if (line.empty()) return defaultValue;

        for (char& ch : line) {
            if (ch == ',') ch = '.';
        }
        try {
            size_t pos = 0;
            double value = stod(line, &pos);
            if (pos == line.size() && isfinite(value)) return value;
        }
        catch (const exception&) {
        }
        cout << "Введите корректное конечное число.\n";
    }
}

void reportStability(double a1, double a2) {
    // Characteristic equation: z^2 - a1*z - a2 = 0.
    const complex<double> discriminant(a1 * a1 + 4.0 * a2, 0.0);
    const complex<double> rootPart = sqrt(discriminant);
    const complex<double> root1 = (a1 + rootPart) / 2.0;
    const complex<double> root2 = (a1 - rootPart) / 2.0;
    const bool stable = abs(root1) < 1.0 && abs(root2) < 1.0;

    cout << "\n--- Проверка устойчивости Model 1.3 ---\n";
    cout << "Характеристическое уравнение: z^2 - a1*z - a2 = 0\n";
    cout << fixed << setprecision(6);
    cout << "Корень z1 = " << root1 << ", |z1| = " << abs(root1) << '\n';
    cout << "Корень z2 = " << root2 << ", |z2| = " << abs(root2) << '\n';
    cout << "Критерий асимптотической устойчивости: |z1| < 1 и |z2| < 1.\n";
    if (stable) {
        cout << "Результат: линейная модель асимптотически устойчива.\n";
    }
    else {
        cout << "ПРЕДУПРЕЖДЕНИЕ: модель неустойчива или находится на границе устойчивости. "
            << "Результаты могут расходиться.\n";
    }
}

int main() {
    #ifdef _WIN32
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);
    #endif
    cout << "==============================================\n";
    cout << "Моделирование объекта управления\n";
    cout << "==============================================\n";
    cout << "Выберите модель, которую нужно запустить:\n";
    cout << "1. Model 1.3 - линейная модель второго порядка\n";
    cout << "2. Model 2.7 - реле с гистерезисом\n";
    cout << "3. Model 3.1 - дифференциальное уравнение\n";

    const int modelChoice = readInt("Номер модели", 1, 1, 3);

    cout << "\nВыберите входное воздействие для моделирования:\n";
    cout << "1. Ступенчатое: u(tau) = 1\n";
    cout << "2. Импульсное: u(0) = 1, далее u(tau) = 0\n";
    cout << "3. Гармоническое: u(tau) = sin(tau)\n";
    const int inputChoice = readInt("Номер входного сигнала", 1, 1, 3);

    InputType selectedInput = InputType::Step;
    if (inputChoice == 2) selectedInput = InputType::Impulse;
    else if (inputChoice == 3) selectedInput = InputType::Harmonic;

    const int n = readInt("Количество шагов симуляции n", 50, 1, 100000);

    double dt = 0.1;
    unique_ptr<Model> selectedModel;

    if (modelChoice == 1) {
        cout << "\nПараметры Model 1.3 (линейная модель):\n";
        const double a1 = readDouble("a1", 1.2);
        const double a2 = readDouble("a2", -0.32);
        const double b = readDouble("b", 0.5);

        reportStability(a1, a2);
        selectedModel = make_unique<LinearModel13>(a1, a2, b);
    }
    else if (modelChoice == 2) {
        cout << "\nПараметры Model 2.7 (реле с гистерезисом):\n";
        const double a = readDouble("a", 0.8);
        const double b = readDouble("b", 0.4);
        double epsilon = readDouble("epsilon (порог гистерезиса)", 0.1);
        if (epsilon < 0.0) {
            cout << "epsilon не может быть отрицательным. Используется epsilon = 0.1.\n";
            epsilon = 0.1;
        }
        selectedModel = make_unique<RelayModel27>(a, b, epsilon);
    }
    else {
        cout << "\nПараметры Model 3.1 (dy/dt = -a*y):\n";
        const double a = readDouble("a", 0.4);
        dt = readDouble("Шаг интегрирования dt", 0.1);
        if (dt <= 0.0) {
            cout << "Шаг dt должен быть больше нуля. Используется dt = 0.1.\n";
            dt = 0.1;
        }
        selectedModel = make_unique<DifferentialModel31>(a);
    }

    vector<ResultRow> rows = Simulator::run(*selectedModel, selectedInput, n, dt);

    ResultWriter::printTable(rows);
    const string csvPath = string(PROJECT_ROOT) + "/results.csv";
    if (ResultWriter::saveCsv(rows, csvPath)) {
        cout << "\nТаблица сохранена в файл: " << csvPath << '\n';
    }
    else {
        cerr << "Ошибка: не удалось записать файл " << csvPath << '\n';
        return 1;
    }

    cout << "\nВыбранное воздействие: " << inputName(selectedInput) << '\n';
    if (modelChoice == 3) {
        cout << "Примечание: Model 3.1 описывает dy/dt = -a*y и не содержит u.\n";
        cout << "Поэтому выбранный входной сигнал не влияет на выход модели.\n";
    }

    cout << "\nСимуляция завершена. Запуск построения графика через py...\n";
    cout.flush();

#ifdef _WIN32
    if (_chdir(PROJECT_ROOT) != 0) {
#else
    if (chdir(PROJECT_ROOT) != 0) {
#endif
        cerr << "Не удалось перейти в папку проекта: " << PROJECT_ROOT << '\n';
        return 1;
    }

#ifdef _WIN32
    const int pythonResult = system("py plot.py");
#else
    const int pythonResult = system("python3 plot.py");
#endif

    if (pythonResult != 0) {
        cerr << "Не удалось запустить plot.py.\n";
        cerr << "Проверьте установку Python и Matplotlib.\n";
#ifdef _WIN32
        cerr << "Установить Matplotlib можно командой: py -m pip install matplotlib\n";
#endif
        return 1;
    }

    return 0;
    }
