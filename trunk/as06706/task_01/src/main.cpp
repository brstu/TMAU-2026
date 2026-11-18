
#define NOMINMAX

#ifdef _WIN32
#include <windows.h>
#endif

#include "Models.h"

#include <algorithm>
#include <cmath>
#include <clocale>
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

struct SimulationRow {
    int tau;
    double input;
    double currentY;
    double nextY;
};

class SimulationEngine {
public:
    std::vector<SimulationRow> run(
        const Model& model,
        const InputSignal& signal,
        int n,
        double initialY
    ) const;
};

bool readDouble(const std::string& prompt, double& value) {
    while (true) {
        std::cout << prompt;

        std::string line;

        if (!std::getline(std::cin, line)) {
            return false;
        }

        std::replace(line.begin(), line.end(), ',', '.');

        std::istringstream parser(line);
        double parsedValue = 0.0;

        if (char extraSymbol = '\0';
            parser >> parsedValue &&
            !(parser >> extraSymbol) &&
            std::isfinite(parsedValue)) {
            value = parsedValue;
            return true;
        }

        std::cout << "Ошибка ввода. Примеры: 0,5; 0.5; -1.\n";
    }
}

void configureConsoleEncoding() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#else
    std::setlocale(LC_ALL, "");
#endif
}

bool readInt(
    const std::string& prompt,
    int minValue,
    int maxValue,
    int& value
) {
    while (true) {
        std::cout << prompt;

        if (std::cin >> value &&
            value >= minValue &&
            value <= maxValue) {
            std::cin.ignore(
                (std::numeric_limits<std::streamsize>::max)(),
                '\n'
            );
            return true;
        }

        if (std::cin.eof()) {
            return false;
        }

        std::cout << "Введите целое число от "
            << minValue << " до " << maxValue << ".\n";

        std::cin.clear();
        std::cin.ignore(
            (std::numeric_limits<std::streamsize>::max)(),
            '\n'
        );
    }
}

std::unique_ptr<InputSignal> chooseInputSignal(int& signalChoice) {
    std::cout << "\nВыберите входное воздействие:\n"
        << "1. Ступенчатое: u(tau) = const\n"
        << "2. Импульсное: u(0) = 1, далее 0\n"
        << "3. Гармоническое: u(tau) = sin(tau)\n";

    if (!readInt("Ваш выбор (1-3): ", 1, 3, signalChoice)) {
        return nullptr;
    }

    if (signalChoice == 1) {
        double amplitude = 1.0;

        if (!readDouble(
            "Введите постоянное воздействие (например, 1 или 0,5): ",
            amplitude)) {
            return nullptr;
        }

        return std::make_unique<StepSignal>(amplitude);
    }

    if (signalChoice == 2) {
        return std::make_unique<ImpulseSignal>();
    }

    return std::make_unique<HarmonicSignal>();
}

std::vector<SimulationRow> SimulationEngine::run(
    const Model& model,
    const InputSignal& signal,
    int n,
    double initialY
) const {
    std::vector<SimulationRow> rows;
    std::vector<double> inputHistory;

    rows.reserve(static_cast<std::size_t>(n));
    inputHistory.reserve(static_cast<std::size_t>(n));

    double currentY = initialY;

    for (int tau = 0; tau < n; ++tau) {
        const double currentInput = signal.value(tau);

        const double nextY = model.calculateNext(
            tau,
            currentY,
            currentInput,
            inputHistory
        );

        if (!std::isfinite(nextY)) {
            std::cout << "\nРасчет остановлен на шаге "
                << tau << ": получено недопустимое значение.\n";
            break;
        }

        rows.push_back({
            tau,
            currentInput,
            currentY,
            nextY
            });

        inputHistory.push_back(currentInput);
        currentY = nextY;
    }

    return rows;
}

void printResults(const std::vector<SimulationRow>& rows) {
    std::cout << "\nРезультаты моделирования:\n";

    std::cout << std::left
        << std::setw(8) << "Шаг"
        << " | "
        << std::setw(14) << "Вход u(tau)"
        << " | "
        << std::setw(16) << "Текущее y(tau)"
        << " | "
        << "Следующее y(tau+1)\n";

    std::cout << std::string(75, '-') << '\n';
    std::cout << std::fixed << std::setprecision(6);

    for (const SimulationRow& row : rows) {
        std::cout << std::right
            << std::setw(8) << row.tau << " | "
            << std::setw(14) << row.input << " | "
            << std::setw(16) << row.currentY << " | "
            << std::setw(18) << row.nextY << '\n';
    }
}

bool saveCsv(
    const std::vector<SimulationRow>& rows,
    const std::filesystem::path& filePath
) {
    std::ofstream file(filePath, std::ios::binary);

    if (!file.is_open()) {
        std::cout << "Не удалось создать файл results.csv.\n";
        return false;
    }

    file << "\xEF\xBB\xBF";
    file << "Шаг,Входное воздействие,Текущее значение y,Следующее значение y\n";
    file << std::setprecision(15);

    for (const SimulationRow& row : rows) {
        file << row.tau << ','
            << row.input << ','
            << row.currentY << ','
            << row.nextY << '\n';
    }

    std::cout << "\nТаблица сохранена в файл: "
        << filePath.string() << '\n';

    return file.good();
}

void launchPythonPlot(
    const char* executableArgv0,
    int modelChoice,
    int signalChoice
) {
    namespace fs = std::filesystem;

    std::error_code error;
    fs::path plotPath;

    if (executableArgv0 != nullptr) {
        fs::path executablePath =
            fs::absolute(executableArgv0, error);

        if (!error) {
            plotPath = executablePath.parent_path() / "plot.py";
        }
    }

    error.clear();

    if (plotPath.empty() || !fs::is_regular_file(plotPath, error)) {
        error.clear();
        plotPath = fs::current_path(error) / "plot.py";
    }

    error.clear();

    if (!fs::is_regular_file(plotPath, error)) {
        std::cout << "Файл plot.py не найден.\n";
        return;
    }

    std::string pythonCommand;

#ifdef _WIN32
    if (std::system("py -3 --version >NUL 2>&1") == 0) {
        pythonCommand = "py -3";
    }
    else if (std::system("python --version >NUL 2>&1") == 0) {
        pythonCommand = "python";
    }
#else
    if (std::system("python3 --version >/dev/null 2>&1") == 0) {
        pythonCommand = "python3";
    }
    else if (std::system("python --version >/dev/null 2>&1") == 0) {
        pythonCommand = "python";
    }
#endif

    if (pythonCommand.empty()) {
        std::cout << "Python 3 не найден. Установите Python и Matplotlib.\n";
        return;
    }

    std::cout << "\nПостроение графиков для выбранной модели...\n";

    const std::string command =
        pythonCommand + " \"" + plotPath.string() + "\" " +
        std::to_string(modelChoice) + " " +
        std::to_string(signalChoice);

    const int result = std::system(command.c_str());

    if (result != 0) {
        std::cout << "Не удалось построить графики.\n"
            << "Установите Matplotlib командой:\n"
            << pythonCommand << " -m pip install matplotlib\n";
    }
}

void runModel(int modelChoice, const char* executableArgv0) {
    int n = 0;

    if (!readInt(
        "Введите количество шагов n (1-100000, например 30): ",
        1,
        100000,
        n)) {
        return;
    }

    double initialY = 0.0;

    if (!readDouble(
        "Введите начальное значение y_0 (например, 0 или -1): ",
        initialY)) {
        return;
    }

    int signalChoice = 0;
    std::unique_ptr<InputSignal> signal = chooseInputSignal(signalChoice);

    if (!signal) {
        return;
    }

    std::unique_ptr<Model> model;

    if (modelChoice == 1) {
        double a = 0.0;
        double b = 0.0;
        int delay = 1;

        if (!readDouble("Введите коэффициент a (например, 0,8): ", a) ||
            !readDouble("Введите коэффициент b (например, 0,2): ", b) ||
            !readInt("Введите задержку k (например, 3): ",
                1, 100000, delay)) {
            return;
        }

        model = std::make_unique<DelayedLinearModel>(a, b, delay);
    }
    else if (modelChoice == 2) {
        double a = 0.0;
        double b = 0.0;

        if (!readDouble("Введите коэффициент a (например, 0,5): ", a) ||
            !readDouble("Введите коэффициент b (например, 1): ", b)) {
            return;
        }

        model = std::make_unique<NonlinearLogModel>(a, b);
    }
    else {
        double a = 0.0;
        double b = 0.0;
        double deltaT = 0.1;

        if (!readDouble("Введите коэффициент a (например, -0,1): ", a) ||
            !readDouble("Введите коэффициент b (например, 1): ", b) ||
            !readDouble(
                "Введите шаг интегрирования delta_t (например, 0,1): ",
                deltaT)) {
            return;
        }

        while (deltaT <= 0.0) {
            std::cout << "Шаг интегрирования должен быть больше нуля.\n";

            if (!readDouble("Введите delta_t: ", deltaT)) {
                return;
            }
        }

        model = std::make_unique<DifferentialCubicModel>(a, b, deltaT);
    }

    std::cout << "\nВыбрана модель: " << model->name() << '\n';
    std::cout << "Входное воздействие: " << signal->name() << '\n';

    if (const std::string stability = model->stabilityInfo();
        !stability.empty()) {
        std::cout << stability << '\n';
    }

    const SimulationEngine simulator;
    const std::vector<SimulationRow> rows =
        simulator.run(*model, *signal, n, initialY);

    if (rows.empty()) {
        std::cout << "Нет результатов для сохранения.\n";
        return;
    }

    // Выводим фактическое число рассчитанных строк, а не исходный аргумент n.
    std::cout << "Количество рассчитанных шагов: " << rows.size()
        << ", начальное значение y_0 = " << initialY << '\n';

    printResults(rows);

    if (saveCsv(rows, "results.csv")) {
        launchPythonPlot(executableArgv0, modelChoice, signalChoice);
    }
}

int main(int argc, char* argv[]) {
    (void)argc;

    configureConsoleEncoding();

    std::cout << "  МОДЕЛИРОВАНИЕ ОБЪЕКТА УПРАВЛЕНИЯ - ВАРИАНТ 2\n"
        << "\nВыберите модель для моделирования:\n"
        << "1. Линейная модель 1.2 (задержка управления)\n"
        << "2. Нелинейная модель 2.4 (косинус и логарифм)\n"
        << "3. Дифференциальная модель 3.6 (кубический рост)\n"
        << "0. Выход\n";

    int choice = 0;

    if (!readInt("Ваш выбор: ", 0, 3, choice) || choice == 0) {
        return 0;
    }

    runModel(choice, argc > 0 ? argv[0] : nullptr);

    return 0;
}
