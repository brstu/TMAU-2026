#include <cmath>
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
#include <windows.h>
#endif

#ifndef PLOT_SCRIPT_PATH
#define PLOT_SCRIPT_PATH "plot_results.py"
#endif

enum class InputType {
    Step = 1,
    Impulse = 2,
    Harmonic = 3
};

class InputSignal {
public:
    static std::string name(InputType type) {
        switch (type) {
        case InputType::Step:     return "Ступенчатое";
        case InputType::Impulse:  return "Импульсное";
        case InputType::Harmonic: return "Гармоническое";
        }
        return "Неизвестное";
    }

    static double value(InputType type, std::size_t tau, double stepValue) {
        switch (type) {
        case InputType::Step:
            return stepValue; 
        case InputType::Impulse:
            return (tau == 0) ? 1.0 : 0.0; 
        case InputType::Harmonic:
            return std::sin(static_cast<double>(tau)); 
        }
        return 0.0;
    }
};

class Model {
public:
    virtual ~Model() = default;
    virtual std::string name() const = 0;

    virtual double nextStep(const std::vector<double>& y,
                            const std::vector<double>& u,
                            std::size_t tau,
                            double dt) const = 0;

    virtual double timeAt(std::size_t tau, double dt) const {
        (void)dt;
        return static_cast<double>(tau);
    }
};

class LinearModel110 : public Model {
private:
    double a1_;
    double b1_;
    double b2_;

public:
    LinearModel110(double a1, double b1, double b2)
        : a1_(a1), b1_(b1), b2_(b2) {}

    std::string name() const override {
        return "Модель 1.10 (линейная)";
    }

    double nextStep(const std::vector<double>& y,
                    const std::vector<double>& u,
                    std::size_t tau,
                    double dt) const override {
        (void)dt;
        const double delayedInput = (tau >= 2) ? u[tau - 2] : 0.0;
        return a1_ * y[tau] + b1_ * u[tau] + b2_ * delayedInput;
    }
};

class NonlinearModel24 : public Model {
private:
    double a_;
    double b_;

public:
    NonlinearModel24(double a, double b) : a_(a), b_(b) {}

    std::string name() const override {
        return "Модель 2.4 (нелинейная)";
    }

    double nextStep(const std::vector<double>& y,
                    const std::vector<double>& u,
                    std::size_t tau,
                    double dt) const override {
        (void)dt;
        return a_ * y[tau] * std::cos(y[tau])
             + b_ * std::log(1.0 + std::abs(u[tau]));
    }
};

class DifferentialModel38 : public Model {
private:
    double a1_;
    double a2_;
    double b_;

public:
    DifferentialModel38(double a1, double a2, double b)
        : a1_(a1), a2_(a2), b_(b) {}

    std::string name() const override {
        return "Модель 3.8 (дифференциальная)";
    }

    double nextStep(const std::vector<double>& y,
                    const std::vector<double>& u,
                    std::size_t tau,
                    double dt) const override {
        const double currentY = y[tau];
        const double derivative = -a1_ * currentY
                                - a2_ * currentY * currentY
                                + b_ * u[tau];
        return currentY + dt * derivative;
    }

    double timeAt(std::size_t tau, double dt) const override {
        return static_cast<double>(tau) * dt;
    }
};

struct SimulationRow {
    std::string model;
    std::string input;
    std::size_t tau;
    double time;
    double u;
    double y;
};

class SimulationRunner {
private:
    double dt_;

public:
    explicit SimulationRunner(double dt) : dt_(dt) {}

    std::vector<SimulationRow> run(const Model& model,
                                   InputType inputType,
                                   std::size_t n,
                                   double y0,
                                   double stepValue) const {
        std::vector y(n + 1, 0.0);
        std::vector u(n + 1, 0.0);
        std::vector<SimulationRow> rows;
        rows.reserve(n + 1);
        y[0] = y0;

        for (std::size_t tau = 0; tau <= n; ++tau) {
            u[tau] = InputSignal::value(inputType, tau, stepValue);
        }

        std::size_t lastIndex = n;
        for (std::size_t tau = 0; tau < n; ++tau) {
            y[tau + 1] = model.nextStep(y, u, tau, dt_);
            if (!std::isfinite(y[tau + 1])) {
                std::cout << "\nПредупреждение: получено некорректное значение на шаге "
                          << (tau + 1) << ". Расчет остановлен.\n";
                lastIndex = tau;
                break;
            }
        }

        for (std::size_t tau = 0; tau <= lastIndex; ++tau) {
            rows.push_back({model.name(), InputSignal::name(inputType), tau,
                            model.timeAt(tau, dt_), u[tau], y[tau]});
        }
        return rows;
    }
};

class CsvExporter {
public:
    static bool save(const std::string& fileName,
                     const std::vector<SimulationRow>& rows) {
        std::ofstream file(fileName, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Не удалось создать файл " << fileName << ".\n";
            return false;
        }

        file << "\xEF\xBB\xBF";
        file << "Model;Input;Tau;Time;u_tau;y_tau\n";
        file << std::setprecision(10);
        for (const SimulationRow& row : rows) {
            file << row.model << ';'
                 << row.input << ';'
                 << row.tau << ';'
                 << row.time << ';'
                 << row.u << ';'
                 << row.y << '\n';
        }
        return true;
    }
};

class TablePrinter {
public:
    static void print(const std::vector<SimulationRow>& rows) {
        if (rows.empty()) {
            std::cout << "Нет строк для отображения.\n";
            return;
        }

        std::cout << "\n" << rows.front().model
            << " | Вход: " << rows.front().input << '\n';

        std::cout << std::left
            << std::setw(8) << "tau"
            << std::setw(14) << "Время"
            << std::setw(16) << "u(tau)"
            << std::setw(16) << "y(tau)"
            << '\n';

        std::cout << std::string(54, '-') << '\n';
        std::cout << std::fixed << std::setprecision(5);

        for (const SimulationRow& row : rows) {
            std::cout << std::left
                << std::setw(8) << row.tau
                << std::setw(14) << row.time
                << std::setw(16) << row.u
                << std::setw(16) << row.y
                << '\n';
        }

    }
};

double readDouble(const std::string& prompt, double defaultValue,
                  double minValue = -1000000.0, double maxValue = 1000000.0) {
    while (true) {
        std::cout << prompt << " [по умолчанию " << defaultValue << "]: ";
        std::string line;
        if (!std::getline(std::cin, line)) {
            return defaultValue;
        }
        if (line.empty()) {
            return defaultValue;
        }

        std::stringstream parser(line);
        double value = 0.0;
        if (char extra = '\0';
            (parser >> value) &&
            !(parser >> extra) &&
            std::isfinite(value) &&
            value >= minValue &&
            value <= maxValue) {
            return value;
        }
        std::cout << "Ошибка: введите число в диапазоне от " << minValue
                  << " до " << maxValue << ".\n";
    }
}

std::size_t readStepCount() {
    while (true) {
        std::cout << "Количество шагов n (1..100000) [20]: ";
        std::string line;
        if (!std::getline(std::cin, line)) {
            return 20;
        }
        if (line.empty()) {
            return 20;
        }

        std::stringstream parser(line);
        long long value = 0;
        if (char extra = '\0';
            (parser >> value) &&
            !(parser >> extra)) {
            return static_cast<std::size_t>(value);
        }
        std::cout << "Ошибка: введите целое число от 1 до 100000.\n";
    }
}

int readChoice(const std::string& title, int minValue, int maxValue) {
    while (true) {
        std::cout << title;
        std::string line;
        if (!std::getline(std::cin, line)) {
            return minValue;
        }

        std::stringstream parser(line);
        int value = 0;

        if (char extra = '\0';
            (parser >> value) &&
            !(parser >> extra) &&
            value >= minValue &&
            value <= maxValue) {
            return value;
        }
        std::cout << "Ошибка: выберите пункт от " << minValue
                  << " до " << maxValue << ".\n";
    }
}

bool runPythonPlot(const std::string& csvFile) {
    std::cout.flush();
    const std::string script = std::string("\"") + PLOT_SCRIPT_PATH + "\"";
    const std::string csv = std::string("\"") + csvFile + "\"";
    std::string command;

#ifdef _WIN32
    command = "py -3 " + script + " " + csv;
    if (std::system(command.c_str()) == 0) {
        return true;
    }
    command = "python " + script + " " + csv;
#else
    command = "python3 " + script + " " + csv;
    if (std::system(command.c_str()) == 0) {
        return true;
    }
    command = "python " + script + " " + csv;
#endif

    if (std::system(command.c_str()) == 0) {
        return true;
    }

    std::cout << "Не удалось автоматически построить график.\n";
    return false;
}

int main() {
#ifdef _WIN32
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);
#endif

    std::cout << "Выбор модели, входного сигнала и параметров с клавиатуры\n";
    std::cout << "Выберите модель для моделирования:\n"
              << "1. Model 1.10 — линейная: y(t+1)=a1*y(t)+b1*u(t)+b2*u(t-2)\n"
              << "2. Model 2.4 — нелинейная: y(t+1)=a*y(t)*cos(y(t))+b*ln(1+|u(t)|)\n"
              << "3. Model 3.8 — дифференциальная: dy/dt=-a1*y-a2*y^2+b*u\n";
    const int modelChoice = readChoice("Ваш выбор (1-3): ", 1, 3);

    std::cout << "\nВыберите входное воздействие u(t):\n"
              << "1. Ступенчатое: u(t)=const\n"
              << "2. Импульсное: u(0)=1, далее 0\n"
              << "3. Гармоническое: u(t)=sin(t)\n";
    const auto inputType = static_cast<InputType>(
        readChoice("Ваш выбор (1-3): ", 1, 3));

    const double y0 = readDouble("Начальное значение y(0)", 0.0, -1000.0, 1000.0);
    double stepValue = 1.0;
    if (inputType == InputType::Step) {
        stepValue = readDouble("Постоянное значение входа u(t)", 1.0);
    }

    std::unique_ptr<Model> model;
    double dt = 0.1;
    if (modelChoice == 1) {
        std::cout << "\nПараметры линейной модели 1.10:\n";
        const double a1 = readDouble("Коэффициент a1", 0.8);
        const double b1 = readDouble("Коэффициент b1", 0.5);
        const double b2 = readDouble("Коэффициент b2", 0.2);
        model = std::make_unique<LinearModel110>(a1, b1, b2);
    } else if (modelChoice == 2) {
        std::cout << "\nПараметры нелинейной модели 2.4:\n";
        const double a = readDouble("Коэффициент a", 0.7);
        const double b = readDouble("Коэффициент b", 0.8);
        model = std::make_unique<NonlinearModel24>(a, b);
    } else {
        std::cout << "\nПараметры дифференциальной модели 3.8:\n";
        const double a1 = readDouble("Коэффициент a1", 0.5);
        const double a2 = readDouble("Коэффициент a2", 0.05);
        const double b = readDouble("Коэффициент b", 1.0);
        dt = readDouble("Шаг интегрирования delta_t", 0.1, 0.000001, 10.0);
        std::cout << "Для модели 3.8 используется метод Эйлера.\n";
        model = std::make_unique<DifferentialModel38>(a1, a2, b);
    }

    const std::size_t n = readStepCount();
    SimulationRunner runner(dt);
    const std::vector<SimulationRow> rows = runner.run(*model, inputType, n, y0, stepValue);

    TablePrinter::print(rows);

    if (const std::string csvFile =
        (std::filesystem::current_path() / "simulation.csv").string();
        CsvExporter::save(csvFile, rows)) {
        std::cout << "\nРезультаты сохранены в файл:\n"
            << csvFile << '\n';
        std::cout << "\nЗапускаю Python для построения графиков...\n";
        runPythonPlot(csvFile);
    }

    std::cout << "\nРасчет завершен.\n";
    return 0;
}
