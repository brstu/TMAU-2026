#include <iostream>
#include <vector>
#include <fstream>
#include <cmath>
#include <string>
#include <algorithm>
#include <iomanip>
#include <stdexcept>
#include "models.h"

std::vector<double> generateInputSignal(int type, int steps) {
    std::vector u(steps, 0.0);
    for (int t = 0; t < steps; ++t) {
        if (type == 1) {
            u[t] = 1.0;
        } else if (type == 2) {
            u[t] = (t == 0) ? 1.0 : 0.0;
        } else if (type == 3) {
            u[t] = std::sin(static_cast<double>(t));
        }
    }
    return u;
}

void printAndSaveCSV(Model& model, const std::string& filename, const std::string& title, 
                    const std::vector<double>& input) {
    model.reset();
    auto output = model.simulate(input);

    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return;
    }

    file << "Time,Input,Output\n";
    for (size_t t = 0; t < input.size(); ++t) {
        file << t << "," << input[t] << "," << output[t] << "\n";
    }
    file.close();

    std::cout << "\n==========================================" << std::endl;
    std::cout << "  Таблица результатов: " << title << std::endl;
    std::cout << "==========================================" << std::endl;
    std::cout << std::setw(8) << "Time" << " | " 
              << std::setw(10) << "Input" << " | " 
              << std::setw(12) << "Output" << std::endl;
    std::cout << "------------------------------------------" << std::endl;

    size_t print_limit = std::min(output.size(), static_cast<size_t>(100));
    for (size_t t = 0; t < print_limit; ++t) {
        std::cout << std::setw(8) << t << " | " 
                  << std::setw(10) << std::fixed << std::setprecision(4) << input[t] << " | " 
                  << std::setw(12) << std::fixed << std::setprecision(4) << output[t] << std::endl;
    }
    if (output.size() > 100) {
        std::cout << "  ... [показано 100 шагов из " << output.size() << "]" << std::endl;
    }
    std::cout << "Сохранено в файл: " << filename << std::endl;
}

double getDoubleInput(const std::string& prompt, double default_val) {
    std::cout << prompt << " [по умолчанию " << default_val << "]: ";
    std::string line;
    std::getline(std::cin, line);
    if (line.empty()) {
        return default_val;
    }
    try {
        return std::stod(line);
    } catch (const std::invalid_argument& e) {
        std::cerr << "Некорректный ввод, используем значение по умолчанию: " << e.what() << std::endl;
        return default_val;
    } catch (const std::out_of_range& e) {
        std::cerr << "Число вне диапазона, используем значение по умолчанию: " << e.what() << std::endl;
        return default_val;
    }
}

int parseSteps(int argc, char* argv[]) {
    int parsed_steps = 100;
    if (argc > 1) {
        try {
            parsed_steps = std::stoi(argv[1]);
        } catch (const std::invalid_argument& e) {
            std::cerr << "Ошибка аргумента командной строки: " << e.what() << std::endl;
            parsed_steps = 100;
        } catch (const std::out_of_range& e) {
            std::cerr << "Число вне диапазона: " << e.what() << std::endl;
            parsed_steps = 100;
        }
    } else {
        std::cout << "Количество шагов симуляции (n) [по умолчанию 100]: ";
        std::string input_str;
        std::getline(std::cin, input_str);
        if (!input_str.empty()) {
            try {
                parsed_steps = std::stoi(input_str);
            } catch (const std::invalid_argument& e) {
                std::cerr << "Некорректный ввод шагов, используем 100: " << e.what() << std::endl;
                parsed_steps = 100;
            } catch (const std::out_of_range& e) {
                std::cerr << "Число вне диапазона шагов, используем 100: " << e.what() << std::endl;
                parsed_steps = 100;
            }
        }
    }
    return std::clamp(parsed_steps, 1, 10000);
}

void selectOptions(int& model_choice, int& signal_choice) {
    std::cout << "\nВыберите модель:\n"
              << "1. Линейная модель (LinearModel1_4)\n"
              << "2. Нелинейная модель (NonlinearModel2_8)\n"
              << "3. Дифференциальная модель (DiffModel3_2)\n"
              << "4. Все модели (по умолчанию)\n"
              << "Ваш выбор [1-4]: ";
    std::string input_str;
    std::getline(std::cin, input_str);
    if (!input_str.empty()) {
        try {
            model_choice = std::stoi(input_str);
        } catch (const std::invalid_argument& e) {
            std::cerr << "Ошибка выбора модели: " << e.what() << std::endl;
        } catch (const std::out_of_range& e) {
            std::cerr << "Значение выбора модели вне диапазона: " << e.what() << std::endl;
        }
    }

    std::cout << "\nВыберите входное воздействие:\n"
              << "1. Ступенчатое (Step)\n"
              << "2. Импульсное (Impulse)\n"
              << "3. Гармоническое (Harmonic)\n"
              << "4. Все воздействия (по умолчанию)\n"
              << "Ваш выбор [1-4]: ";
    std::getline(std::cin, input_str);
    if (!input_str.empty()) {
        try {
            signal_choice = std::stoi(input_str);
        } catch (const std::invalid_argument& e) {
            std::cerr << "Ошибка выбора сигнала: " << e.what() << std::endl;
        } catch (const std::out_of_range& e) {
            std::cerr << "Значение выбора сигнала вне диапазона: " << e.what() << std::endl;
        }
    }
}

int main(int argc, char* argv[]) {
    int model_choice = 4;
    int signal_choice = 4;

    int steps = parseSteps(argc, argv);

    if (argc <= 1) {
        selectOptions(model_choice, signal_choice);
    }

    double lin_a = 0.8;
    double lin_b = 0.2;
    double nonlin_a = 0.5;
    double nonlin_b = 0.3;
    double nonlin_c = 0.1;
    double diff_b = 0.5;
    double diff_dt = 0.1;

    if (argc <= 1) {
        std::cout << "\n--- Настройка коэффициентов моделей ---" << std::endl;
        if (model_choice == 1 || model_choice == 4) {
            std::cout << "\nПараметры линейной модели:" << std::endl;
            lin_a = getDoubleInput("  Коэффициент a", 0.8);
            lin_b = getDoubleInput("  Коэффициент b", 0.2);
        }
        if (model_choice == 2 || model_choice == 4) {
            std::cout << "\nПараметры нелинейной модели:" << std::endl;
            nonlin_a = getDoubleInput("  Коэффициент a", 0.5);
            nonlin_b = getDoubleInput("  Коэффициент b", 0.3);
            nonlin_c = getDoubleInput("  Коэффициент c", 0.1);
        }
        if (model_choice == 3 || model_choice == 4) {
            std::cout << "\nПараметры дифференциальной модели:" << std::endl;
            diff_b = getDoubleInput("  Коэффициент b", 0.5);
            diff_dt = getDoubleInput("  Шаг dt", 0.1);
        }
    }

    LinearModel1_4 linearModel(lin_a, lin_b);
    NonlinearModel2_8 nonlinearModel(nonlin_a, nonlin_b, nonlin_c);
    DiffModel3_2 diffModel(diff_b, diff_dt);

    std::vector<std::string> signalNames = {"step", "impulse", "harmonic"};

    for (int sigType = 1; sigType <= 3; ++sigType) {
        if (signal_choice != 4 && signal_choice != sigType) {
            continue;
        }

        auto input = generateInputSignal(sigType, steps);
        std::string sigName = signalNames[sigType - 1];

        if (model_choice == 1 || model_choice == 4) {
            printAndSaveCSV(linearModel, "linear_" + sigName + ".csv", "Linear Model (" + sigName + ")", input);
        }
        if (model_choice == 2 || model_choice == 4) {
            printAndSaveCSV(nonlinearModel, "nonlinear_" + sigName + ".csv", "Nonlinear Model (" + sigName + ")", input);
        }
        if (model_choice == 3 || model_choice == 4) {
            printAndSaveCSV(diffModel, "diff_" + sigName + ".csv", "Diff Model (" + sigName + ")", input);
        }
    }

    std::cout << "\nSimulation completed successfully!" << std::endl;
    return 0;
}