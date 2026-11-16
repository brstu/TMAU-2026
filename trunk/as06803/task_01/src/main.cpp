#include "Model.h"

#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <locale>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#else
#include <limits.h>
#include <unistd.h>
#endif

struct SimulationPoint {
    int tau;
    double input;
    double output;
};

int readInt(const std::string& message, int defaultValue, int minValue, int maxValue) {
    while (true) {
        std::cout << message << " [по умолчанию " << defaultValue << "]: ";
        std::string line;
        std::getline(std::cin, line);

        if (line.empty()) {
            return defaultValue;
        }

        std::stringstream stream(line);
        int value;
        char extra;
        if (stream >> value && !(stream >> extra) && value >= minValue && value <= maxValue) {
            return value;
        }

        std::cout << "Введите целое число от " << minValue << " до " << maxValue << ".\n";
    }
}

double readDouble(const std::string& message, double defaultValue) {
    while (true) {
        std::cout << message << " [по умолчанию " << defaultValue << "]: ";
        std::string line;
        std::getline(std::cin, line);

        if (line.empty()) {
            return defaultValue;
        }

        for (char& symbol : line) {
            if (symbol == ',') {
                symbol = '.';
            }
        }

        std::stringstream stream(line);
        stream.imbue(std::locale::classic());
        double value;
        char extra;
        if (stream >> value && !(stream >> extra)) {
            return value;
        }

        std::cout << "Введите корректное число.\n";
    }
}

std::unique_ptr<Model> createModel(int choice, double& deltaT) {
    if (choice == 1) {
        std::cout << "\nModel 1.8: y(tau+1) = a1*y(tau) + a2*y(tau-1)"
                  << " + b1*u(tau) + b2*u(tau-1)\n";
        const double a1 = readDouble("Коэффициент a1", 0.5);
        const double a2 = readDouble("Коэффициент a2", 0.2);
        const double b1 = readDouble("Коэффициент b1", 0.3);
        const double b2 = readDouble("Коэффициент b2", 0.1);
        return std::unique_ptr<Model>(new LinearModel18(a1, a2, b1, b2));
    }

    if (choice == 2) {
        std::cout << "\nModel 2.10: y(tau+1) = a*tanh(y(tau)) + b*u(tau)^3\n";
        const double a = readDouble("Коэффициент a", 0.8);
        const double b = readDouble("Коэффициент b", 0.2);
        return std::unique_ptr<Model>(new NonlinearModel210(a, b));
    }

    std::cout << "\nModel 3.2: dy/dt = b*u, численное решение методом Эйлера\n";
    const double b = readDouble("Коэффициент b", 0.5);
    do {
        deltaT = readDouble("Шаг интегрирования delta_t", 0.1);
        if (deltaT <= 0.0) {
            std::cout << "Шаг интегрирования должен быть больше нуля.\n";
        }
    } while (deltaT <= 0.0);
    return std::unique_ptr<Model>(new DifferentialModel32(b));
}

std::vector<SimulationPoint> runSimulation(const Model& model,
                                           InputSignal signal,
                                           int n,
                                           double y0,
                                           double amplitude,
                                           double deltaT) {
    std::vector<SimulationPoint> points;
    points.reserve(static_cast<std::size_t>(n) + 1);

    double yCurrent = y0;
    double yPrevious = 0.0;
    points.push_back({0, getInputValue(signal, 0, amplitude), yCurrent});

    for (int tau = 0; tau < n; ++tau) {
        const double uCurrent = getInputValue(signal, tau, amplitude);
        const double uPrevious = tau > 0
            ? getInputValue(signal, tau - 1, amplitude)
            : 0.0;

        const double yNext = model.nextStep(
            yCurrent, yPrevious, uCurrent, uPrevious, deltaT
        );

        yPrevious = yCurrent;
        yCurrent = yNext;
        points.push_back({tau + 1, getInputValue(signal, tau + 1, amplitude), yCurrent});
    }

    return points;
}

void printTable(const std::vector<SimulationPoint>& points) {
    std::cout << "\n+--------+----------------------+----------------------+\n";
    std::cout << "| " << std::left << std::setw(6) << "tau"
              << " | " << std::setw(20) << "u(tau)"
              << " | " << std::setw(20) << "y(tau)" << " |\n";
    std::cout << "+--------+----------------------+----------------------+\n";

    for (const SimulationPoint& point : points) {
        std::cout << "| " << std::right << std::setw(6) << point.tau
                  << " | " << std::setw(20) << point.input
                  << " | " << std::setw(20) << point.output << " |\n";
    }
    std::cout << "+--------+----------------------+----------------------+\n";
}

bool saveCsv(const std::vector<SimulationPoint>& points,
             const std::string& modelName,
             const std::string& signalName,
             const std::string& fileName) {
    std::ofstream file(fileName.c_str(), std::ios::binary);
    if (!file) {
        return false;
    }

    file << "\xEF\xBB\xBF";
    file.imbue(std::locale::classic());
    file << "model,signal,tau,u_tau,y_tau\n";
    file << std::setprecision(12);

    for (const SimulationPoint& point : points) {
        file << '"' << modelName << "\",\"" << signalName << "\"," 
             << point.tau << ',' << point.input << ',' << point.output << '\n';
    }

    return static_cast<bool>(file);
}

std::string getExecutableDirectory(const char* argv0) {
#ifdef _WIN32
    char buffer[MAX_PATH];
    const DWORD length = GetModuleFileNameA(NULL, buffer, MAX_PATH);
    if (length > 0 && length < MAX_PATH) {
        const std::string executablePath(buffer, length);
        const std::string::size_type position = executablePath.find_last_of("\\/");
        if (position != std::string::npos) {
            return executablePath.substr(0, position);
        }
    }
#else
    char buffer[PATH_MAX];
    if (realpath(argv0, buffer) != NULL) {
        const std::string executablePath(buffer);
        const std::string::size_type position = executablePath.find_last_of('/');
        if (position != std::string::npos) {
            return executablePath.substr(0, position);
        }
    }
#endif

    const std::string executablePath = argv0 == NULL ? "model_simulator" : argv0;
    const std::string::size_type position = executablePath.find_last_of("\\/");
    if (position == std::string::npos) {
        return ".";
    }
    return executablePath.substr(0, position);
}

std::string joinPath(const std::string& directory, const std::string& fileName) {
#ifdef _WIN32
    const char separator = '\\';
#else
    const char separator = '/';
#endif
    if (directory.empty()) {
        return fileName;
    }
    if (directory[directory.size() - 1] == '/' || directory[directory.size() - 1] == '\\') {
        return directory + fileName;
    }
    return directory + separator + fileName;
}

bool fileExists(const std::string& fileName) {
    std::ifstream file(fileName.c_str());
    return static_cast<bool>(file);
}

std::string findPlotScript(const std::string& executableDirectory) {
    const std::string executablePath = joinPath(executableDirectory, "plot.py");
    if (fileExists(executablePath)) {
        return executablePath;
    }

#ifdef PLOT_SCRIPT_SOURCE_PATH
    const std::string sourcePath = PLOT_SCRIPT_SOURCE_PATH;
    if (fileExists(sourcePath)) {
        return sourcePath;
    }
#endif

    return executablePath;
}

std::string getSourceDirectory(const std::string& executableDirectory) {
#ifdef PLOT_SCRIPT_SOURCE_PATH
    const std::string sourcePath = PLOT_SCRIPT_SOURCE_PATH;
    const std::string::size_type position = sourcePath.find_last_of("\\/");
    if (position != std::string::npos) {
        return sourcePath.substr(0, position);
    }
#endif
    return executableDirectory;
}

std::string shellQuote(const std::string& value) {
    std::string result = "'";
    for (char symbol : value) {
        if (symbol == '\'') {
            result += "'\\''";
        } else {
            result += symbol;
        }
    }
    result += "'";
    return result;
}

int runPythonPlot(const std::string& scriptPath, const std::string& csvPath,
                  const std::string& imagePath) {
    std::cout << "\nЗапуск Python для построения графика...\n";
    std::cout.flush();

    std::ifstream scriptFile(scriptPath.c_str());
    if (!scriptFile) {
        std::cerr << "Не найден Python-файл: " << scriptPath << '\n';
        return 1;
    }
    scriptFile.close();

#ifdef _WIN32
    const std::string script = "\"" + scriptPath + "\"";
    const std::string csv = "\"" + csvPath + "\"";
    const std::string image = "\"" + imagePath + "\"";
    if (std::system("py -3 --version >nul 2>nul") == 0) {
        return std::system(("py -3 " + script + " " + csv + " --output " + image).c_str());
    }
    return std::system(("python " + script + " " + csv + " --output " + image).c_str());
#else
    const std::string script = shellQuote(scriptPath);
    const std::string csv = shellQuote(csvPath);
    const std::string image = shellQuote(imagePath);
    return std::system(("python3 " + script + " " + csv + " --output " + image).c_str());
#endif
}

int main(int argc, char* argv[]) {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
    std::cout.imbue(std::locale::classic());

    std::cout << "==============================================\n";
    std::cout << "ЛАБОРАТОРНАЯ РАБОТА — ВАРИАНТ 8\n";
    std::cout << "Моделирование управляемого объекта\n";
    std::cout << "==============================================\n\n";

    std::cout << "Выберите модель:\n";
    std::cout << "1. Model 1.8 — линейная модель\n";
    std::cout << "2. Model 2.10 — нелинейная модель\n";
    std::cout << "3. Model 3.2 — дифференциальное уравнение\n";
    const int modelChoice = readInt("Номер модели", 1, 1, 3);

    std::cout << "\nВыберите входное воздействие:\n";
    std::cout << "1. Ступенчатое: u(tau) = A\n";
    std::cout << "2. Импульсное: u(0) = 1, u(tau > 0) = 0\n";
    std::cout << "3. Гармоническое: u(tau) = sin(tau)\n";
    const int signalChoice = readInt("Номер входного воздействия", 1, 1, 3);
    const InputSignal signal = static_cast<InputSignal>(signalChoice);

    const int n = readInt("Количество шагов симуляции n", 20, 1, 10000);
    const double y0 = readDouble("Начальное значение y(0)", 0.0);

    double deltaT = 1.0;
    std::unique_ptr<Model> model = createModel(modelChoice, deltaT);
    const double amplitude = signal == InputSignal::Step
        ? readDouble("Амплитуда ступенчатого воздействия A", 1.0)
        : 1.0;

    std::cout << "\nВыбрана модель: " << model->name() << '\n';
    std::cout << "Выбранный сигнал: " << getInputSignalName(signal) << '\n';

    if (modelChoice == 1) {
        std::cout << "\nПроверка устойчивости Model 1.8:\n";
        std::cout << model->stabilityInfo() << '\n';
        if (model->isStable()) {
            std::cout << "Система устойчива: оба корня находятся внутри единичной окружности.\n";
        } else {
            std::cout << "ПРЕДУПРЕЖДЕНИЕ: система неустойчива или находится на границе устойчивости.\n";
        }
    }

    const std::vector<SimulationPoint> points = runSimulation(
        *model, signal, n, y0, amplitude, deltaT
    );

    printTable(points);

    const std::string executableDirectory = getExecutableDirectory(
        argc > 0 ? argv[0] : "model_simulator"
    );
    const std::string outputDirectory = getSourceDirectory(executableDirectory);
    const std::string csvFile = joinPath(outputDirectory, "results.csv");
    const std::string imageFile = joinPath(outputDirectory, "results_plot.png");
    const std::string plotFile = findPlotScript(executableDirectory);
    const std::string modelName = model->name();
    const std::string signalName = getInputSignalName(signal);

    if (!saveCsv(points, modelName, signalName, csvFile)) {
        std::cerr << "Не удалось сохранить результаты в " << csvFile << ".\n";
        return 1;
    }

    std::cout << "\nРезультаты сохранены в файл " << csvFile << ".\n";

    if (runPythonPlot(plotFile, csvFile, imageFile) != 0) {
        std::cout << "Не удалось построить график. Установите Python и Matplotlib:\n";
        std::cout << "python -m pip install matplotlib\n";
    }

    std::cout << "\nМоделирование завершено.\n";
    return 0;
}
