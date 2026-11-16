#include <complex>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "csv_writer.hpp"
#include "diff_model.hpp"
#include "input_signal.hpp"
#include "linear_model.hpp"
#include "nonlinear_model.hpp"
#include "simulator.hpp"
#include "stability_analyzer.hpp"

// Чтение целого числа; пустая строка — использовать значение по умолчанию.
int read_int(const std::string& prompt, int default_value) {
    std::cout << prompt << " [" << default_value << "]: ";
    std::string line;
    std::getline(std::cin, line);

    if (line.empty()) {
        return default_value;
    }

    std::istringstream stream(line);
    if (int value; stream >> value) {
        return value;
    }
    return default_value;
}

// Чтение вещественного числа с заменой запятой на точку;
// пустая строка — использовать значение по умолчанию.
double read_double(const std::string& prompt, double default_value) {
    std::cout << prompt << " [" << default_value << "]: ";
    std::string line;
    std::getline(std::cin, line);

    if (line.empty()) {
        return default_value;
    }

    for (char& symbol : line) {
        if (symbol == ',') {
            symbol = '.';
        }
    }

    std::istringstream stream(line);
    if (double value; stream >> value) {
        return value;
    }
    return default_value;
}

// Табличный вывод: первые 10 строк, многоточие и последняя строка.
void print_table(const std::vector<SimulationPoint>& points) {
    std::cout << "tau\t| u\t\t| y\n";
    std::cout << "--------------------------------------\n";

    const std::size_t count = points.size();
    for (std::size_t i = 0; i < count; ++i) {
        const bool visible = i < 10 || i + 1 == count;
        if (visible) {
            std::cout << points[i].tau << "\t| " << points[i].u << "\t| "
                      << points[i].y << '\n';
        } else if (i == 10) {
            std::cout << "...\t| ...\t| ...\n";
        }
    }
}

// Прогон линейной модели с анализом устойчивости до начала симуляции.
void run_linear_model(SignalType type, int steps) {
    std::cout << "\n=== Model 1.8: y[t+1] = a1*y[t] + a2*y[t-1]"
                 " + b1*u[t] + b2*u[t-1] ===\n";
    const double a1 = read_double("Коэффициент a1", 0.5);
    const double a2 = read_double("Коэффициент a2", 0.2);
    const double b1 = read_double("Коэффициент b1", 0.3);
    const double b2 = read_double("Коэффициент b2", 0.1);

    const StabilityReport report = analyze_stability(a1, a2);
    std::cout << "Корни характеристического уравнения: z1 = " << report.z1
              << ", z2 = " << report.z2
              << ", спектральный радиус = " << report.spectral_radius << '\n';

    if (const std::string warning = stability_warning(report);
        !warning.empty()) {
        std::cout << warning << '\n';
    }

    const auto model = std::make_unique<LinearModel18>(a1, a2, b1, b2);
    const std::vector<SimulationPoint> points = simulate(*model, type, steps);

    print_table(points);
    if (write_csv("task02_model1_8.csv", points)) {
        std::cout << "Данные сохранены в файл task02_model1_8.csv\n";
    }
}

// Прогон нелинейной модели с насыщением.
void run_nonlinear_model(SignalType type, int steps) {
    std::cout << "\n=== Model 2.2: y[t+1] = a*y[t] + b*sat(u[t]) ===\n";
    const double a = read_double("Коэффициент a", 0.7);
    const double b = read_double("Коэффициент b", 0.5);
    const double u_min = read_double("Граница насыщения u_min", -0.5);
    const double u_max = read_double("Граница насыщения u_max", 0.5);

    const auto model =
        std::make_unique<NonlinearModel22>(a, b, u_min, u_max);
    const std::vector<SimulationPoint> points = simulate(*model, type, steps);

    print_table(points);
    if (write_csv("task02_model2_2.csv", points)) {
        std::cout << "Данные сохранены в файл task02_model2_2.csv\n";
    }
}

// Прогон дифференциальной модели (метод Эйлера).
void run_diff_model(SignalType type, int steps) {
    std::cout << "\n=== Model 3.6: dy/dt = a*y^3 + b*u (метод Эйлера) ===\n";
    const double a = read_double("Коэффициент a", -0.1);
    const double b = read_double("Коэффициент b", 0.5);
    const double dt = read_double("Шаг интегрирования dt", 0.1);

    const auto model = std::make_unique<DiffModel36>(a, b, dt);
    const std::vector<SimulationPoint> points = simulate(*model, type, steps);

    print_table(points);
    if (write_csv("task02_model3_6.csv", points)) {
        std::cout << "Данные сохранены в файл task02_model3_6.csv\n";
    }
}

int main() {
    const int steps = read_int("Количество шагов симуляции n", 30);
    const int signal_choice = read_int(
        "Входное воздействие (1 - ступенчатое, 2 - импульсное, 3 - "
        "гармоническое)", 1);

    SignalType type = SignalType::Step;
    if (signal_choice == 2) {
        type = SignalType::Impulse;
    } else if (signal_choice == 3) {
        type = SignalType::Harmonic;
    }

    std::cout << "Выбранное воздействие: " << signal_name(type) << '\n';

    run_linear_model(type, steps);
    run_nonlinear_model(type, steps);
    run_diff_model(type, steps);

    return 0;
}
