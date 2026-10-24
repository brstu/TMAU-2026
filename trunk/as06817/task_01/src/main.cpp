#include <iostream>
#include <string>
#include <cstdlib>
#include <limits>

#ifdef _WIN32
#include <windows.h>
#include <shellapi.h>
#endif

#include "Model17.h"
#include "Model21.h"
#include "Model35.h"
#include "Simulator.h"

// Вспомогательная функция для ввода параметров с возможностью использовать значение по умолчанию
double getDoubleInput(const std::string& prompt, double default_val) {
    std::cout << prompt << " [по умолчанию " << default_val << "]: ";
    std::string line;
    std::getline(std::cin, line);
    if (line.empty()) {
        return default_val;
    }
    try {
        return std::stod(line);
    } catch (...) {
        std::cout << "Некорректный ввод, используем значение по умолчанию: " << default_val << "\n";
        return default_val;
    }
}

int main() {
#ifdef _WIN32
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);
#endif

    int n;

    std::cout << "Лабораторная работа 1\n";
    std::cout << "ТиМAУ, вариант 17\n";

    std::cout << "Введите количество шагов n: ";
    std::cin >> n;

    if (std::cin.fail() || n <= 0) {
        std::cout << "Количество шагов должно быть больше 0.\n";
        return 1;
    }

    int signalChoice;

    std::cout << "\nВыберите входное воздействие:\n";
    std::cout << "1 - ступенчатое\n";
    std::cout << "2 - импульсное\n";
    std::cout << "3 - гармоническое\n";
    std::cout << "Ваш выбор: ";
    std::cin >> signalChoice;

    if (std::cin.fail() || signalChoice < 1 || signalChoice > 3) {
        std::cout << "Неверный выбор.\n";
        return 1;
    }

    int modelChoice;

    std::cout << "\nВыберите модель:\n";
    std::cout << "1 - модель 1.7\n";
    std::cout << "2 - модель 2.1\n";
    std::cout << "3 - модель 3.5\n";
    std::cout << "Ваш выбор: ";
    std::cin >> modelChoice;

    if (std::cin.fail() || modelChoice < 1 || modelChoice > 3) {
        std::cout << "Неверный выбор модели.\n";
        return 1;
    }

    // Очищаем буфер (используем защищенный вызов (max), чтобы не было конфликтов с Windows API)
    std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');

    SignalType signalType = static_cast<SignalType>(signalChoice);

    std::string csvFile;
    std::string imageFile;
    std::string modelName;

    const std::string plotScript =
        R"(C:\Users\widuxd\Desktop\TMAU-2026-1\trunk\as06817\task_01\src\plot.py)";

    const std::string docDir =
        R"(C:\Users\widuxd\Desktop\TMAU-2026-1\trunk\as06817\task_01\doc)";

    // Ввод параметров в зависимости от выбранной модели
    std::cout << "\n--- Настройка коэффициентов модели ---\n";

    if (modelChoice == 1) {
        double a  = getDoubleInput("Введите коэффициент a", 0.7);
        double b1 = getDoubleInput("Введите коэффициент b1", 0.5);
        double b2 = getDoubleInput("Введите коэффициент b2", 0.2);
        double b3 = getDoubleInput("Введите коэффициент b3", 0.1);

        Model17 model17(a, b1, b2, b3);

        csvFile = "model17.csv";
        imageFile = "model17.png";
        modelName = "модель 1.7";

        Simulator::run(model17, signalType, n, 0.0, csvFile);
    }
    else if (modelChoice == 2) {
        double a = getDoubleInput("Введите коэффициент a", 0.5);
        double b = getDoubleInput("Введите коэффициент b", 0.05);
        double c = getDoubleInput("Введите коэффициент c", 0.8);
        double d = getDoubleInput("Введите коэффициент d", 0.2);

        Model21 model21(a, b, c, d);

        csvFile = "model21.csv";
        imageFile = "model21.png";
        modelName = "модель 2.1";

        Simulator::run(model21, signalType, n, 0.0, csvFile);
    }
    else {
        double b  = getDoubleInput("Введите коэффициент b", 1.0);
        double dt = getDoubleInput("Введите шаг dt", 0.1);

        Model35 model35(b, dt);

        csvFile = "model35.csv";
        imageFile = "model35.png";
        modelName = "модель 3.5";

        Simulator::run(model35, signalType, n, 0.0, csvFile);
    }

    std::string imagePath = docDir + "\\" + imageFile;

    std::cout << "\nМоделирование завершено.\n";
    std::cout << "Результаты: " << csvFile << '\n';
    std::cout << "Построение графика...\n";

    std::string command =
        "python \"" + plotScript + "\" \"" +
        csvFile + "\" \"" + imagePath + "\" \"" +
        modelName + "\"";

    int result = std::system(command.c_str());

    if (result != 0) {
        std::cout << "Ошибка построения графика.\n";
        return 1;
    }

    std::cout << "График сохранён: " << imagePath << '\n';

#ifdef _WIN32
    HINSTANCE openResult = ShellExecuteW(
        NULL,
        L"open",
        std::wstring(imagePath.begin(), imagePath.end()).c_str(),
        NULL,
        NULL,
        SW_SHOWNORMAL
    );

    if (reinterpret_cast<INT_PTR>(openResult) <= 32) {
        std::cout << "Не удалось открыть изображение автоматически.\n";
        std::cout << "Откройте файл вручную: " << imagePath << '\n';
    }
#endif

    return 0;
}