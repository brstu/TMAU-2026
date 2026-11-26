#include <cmath>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "control_system.hpp"
#include "csv_writer.hpp"
#include "diff_model.hpp"
#include "linear_model.hpp"
#include "nonlinear_model.hpp"
#include "pid.hpp"

// Чтение целого числа; пустая строка — значение по умолчанию.
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

// Чтение вещественного числа (запятая заменяется на точку);
// пустая строка — значение по умолчанию.
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

// Табличный вывод процесса: первые 10 строк, многоточие и последняя строка.
void print_table(const std::vector<ControllerPoint>& points) {
    std::cout << "tau\t| w\t| e\t\t| u\t\t| y\n";
    std::cout << "--------------------------------------------------\n";

    const std::size_t count = points.size();
    for (std::size_t i = 0; i < count; ++i) {
        const bool visible = i < 10 || i + 1 == count;
        if (visible) {
            std::cout << points[i].tau << "\t| " << points[i].w << "\t| "
                      << points[i].e << "\t| " << points[i].u << "\t| "
                      << points[i].y << '\n';
        } else if (i == 10) {
            std::cout << "...\t| ...\t| ...\t| ...\t| ...\n";
        }
    }
}

// Итоги прогона: число тактов, конечная ошибка, перерегулирование.
void print_summary(const std::vector<ControllerPoint>& points, double w) {
    double y_max = points.front().y;
    for (const ControllerPoint& point : points) {
        if (point.y > y_max) {
            y_max = point.y;
        }
    }

    const ControllerPoint& last = points.back();
    std::cout << "Тактов: " << points.size()
              << ", конечная |e|: " << std::abs(last.e);

    if (y_max > w && w != 0.0) {
        std::cout << ", перерегулирование: " << (y_max - w) / w * 100.0 << '%';
    }
    std::cout << '\n';
}

// Выбор объекта управления; коэффициенты — с вводом по умолчанию
// (значения варианта 18 из ЛР №1).
std::unique_ptr<Model> select_plant() {
    std::cout << "\nОбъект управления:\n"
              << "  1 — линейная модель 1.8\n"
              << "  2 — нелинейная модель 2.2\n"
              << "  3 — дифференциальное уравнение 3.6\n";
    const int type = read_int("Выбор", 1);

    switch (type) {
    case 2: {
        const double a = read_double("Коэффициент a", 0.7);
        const double b = read_double("Коэффициент b", 0.5);
        const double u_min = read_double("Предел sat u_min", -0.5);
        const double u_max = read_double("Предел sat u_max", 0.5);
        return std::make_unique<NonlinearModel22>(a, b, u_min, u_max);
    }
    case 3: {
        const double a = read_double("Коэффициент a", -0.1);
        const double b = read_double("Коэффициент b", 0.5);
        const double dt = read_double("Шаг интегрирования dt", 0.1);
        return std::make_unique<DiffModel36>(a, b, dt);
    }
    default: {
        const double a1 = read_double("Коэффициент a1", 0.5);
        const double a2 = read_double("Коэффициент a2", 0.2);
        const double b1 = read_double("Коэффициент b1", 0.3);
        const double b2 = read_double("Коэффициент b2", 0.1);
        return std::make_unique<LinearModel18>(a1, a2, b1, b2);
    }
    }
}

// Интерактивный прогон: настройка регулятора, режим симуляции, таблица, CSV.
void run_interactive() {
    auto plant = select_plant();

    const double w = read_double("Задание w (температура)", 25.0);
    const double kp = read_double("Kp", 1.0);
    const double ki = read_double("Ki", 0.5);
    const double kd = read_double("Kd", 1.0);
    const double u_min = read_double("Предел актуатора u_min", -100.0);
    const double u_max = read_double("Предел актуатора u_max", 100.0);

    PidController pid(kp, ki, kd, 1.0, u_min, u_max);
    ControlSystem system(*plant, pid);

    std::vector<ControllerPoint> points;
    std::cout << "\nРежим симуляции:\n  1 — А: фиксированное число тактов\n"
              << "  2 — Б: до установившегося режима (|e| < eps)\n";

    if (const int mode = read_int("Выбор", 2); mode == 1) {
        const int n = read_int("Число тактов n", 60);
        points = run_fixed_steps(system, w, n);
    } else {
        const double eps = read_double("Точность eps", 0.5);
        const int window = read_int("Тактов подряд ниже eps", 3);
        const int max_steps = read_int("Ограничение тактов", 10000);
        points = run_until_steady(system, w, eps, window, max_steps);
    }

    std::cout << '\n';
    print_table(points);
    print_summary(points, w);

    if (write_csv("task03_result.csv", points)) {
        std::cout << "Данные сохранены в файл task03_result.csv\n";
    }
}

/// Сценарий демо-прогона: объект, задание, настройки регулятора, режим.
struct DemoScenario {
    const char* csv;  ///< имя CSV-файла результата
    int plant;        ///< объект: 1 — 1.8, 2 — 2.2, 3 — 3.6
    double w;         ///< задающее воздействие
    double kp;        ///< коэффициент Kp
    double ki;        ///< коэффициент Ki
    double kd;        ///< коэффициент Kd
    int steps;        ///< режим А: число тактов (0 — не используется)
    double eps;       ///< режим Б: точность (< 0 — режим А)
};

std::unique_ptr<Model> make_demo_plant(int plant) {
    switch (plant) {
    case 2:
        return std::make_unique<NonlinearModel22>();
    case 3:
        return std::make_unique<DiffModel36>();
    default:
        return std::make_unique<LinearModel18>();
    }
}

// Прогон одного сценария с записью CSV и кратким итогом в консоль.
void run_demo_scenario(const DemoScenario& scenario) {
    auto plant = make_demo_plant(scenario.plant);

    const double u_max = scenario.plant == 1 ? 100.0 : 10.0;
    PidController pid(scenario.kp, scenario.ki, scenario.kd, 1.0, -u_max,
                      u_max);
    ControlSystem system(*plant, pid);

    std::vector<ControllerPoint> points;
    if (scenario.eps < 0.0) {
        points = run_fixed_steps(system, scenario.w, scenario.steps);
    } else {
        points = run_until_steady(system, scenario.w, scenario.eps);
    }

    write_csv(scenario.csv, points);

    const ControllerPoint& last = points.back();
    std::cout << scenario.csv << ": " << points.size()
              << " тактов, y_end = " << last.y << ", |e_end| = "
              << std::abs(last.e) << '\n';
}

// Демонстрационный прогон всех сценариев отчёта (без интерактива):
// последовательная настройка регулятора, разные задания, оба режима,
// все три объекта. Результаты — CSV-файлы для построения графиков.
void run_demo() {
    std::cout << "Демо-прогон ЛР3 (as06818): сценарии отчёта\n\n";

    const std::vector<DemoScenario> scenarios = {
        // Последовательная настройка на линейной модели 1.8, w = 25.
        {"task03_tune1_p.csv", 1, 25.0, 0.4, 0.0, 0.0, 60, -1.0},
        {"task03_tune2_p.csv", 1, 25.0, 2.0, 0.0, 0.0, 60, -1.0},
        {"task03_tune3_pi.csv", 1, 25.0, 0.4, 0.3, 0.0, 60, -1.0},
        {"task03_tune4_pid.csv", 1, 25.0, 1.0, 0.5, 1.0, 60, -1.0},
        // Итоговые настройки при разных задаваемых температурах.
        {"task03_set25.csv", 1, 25.0, 1.0, 0.5, 1.0, 60, -1.0},
        {"task03_set50.csv", 1, 50.0, 1.0, 0.5, 1.0, 60, -1.0},
        {"task03_set75.csv", 1, 75.0, 1.0, 0.5, 1.0, 60, -1.0},
        // Режим Б: разные точности и задания.
        {"task03_modeb25_eps05.csv", 1, 25.0, 1.0, 0.5, 1.0, 0, 0.5},
        {"task03_modeb25_eps01.csv", 1, 25.0, 1.0, 0.5, 1.0, 0, 0.1},
        {"task03_modeb25_eps005.csv", 1, 25.0, 1.0, 0.5, 1.0, 0, 0.05},
        {"task03_modeb50.csv", 1, 50.0, 1.0, 0.5, 1.0, 0, 0.5},
        {"task03_modeb75.csv", 1, 75.0, 1.0, 0.5, 1.0, 0, 0.5},
        // Настройка на нелинейной модели 2.2 (w = 0.5).
        {"task03_m2_p.csv", 2, 0.5, 0.6, 0.0, 0.0, 60, -1.0},
        {"task03_m2_pi.csv", 2, 0.5, 0.6, 0.2, 0.0, 60, -1.0},
        {"task03_m2_pid.csv", 2, 0.5, 0.6, 0.25, 0.4, 60, -1.0},
        // Настройка на уравнении 3.6 (w = 1.0).
        {"task03_m3_p.csv", 3, 1.0, 1.0, 0.0, 0.0, 150, -1.0},
        {"task03_m3_pi.csv", 3, 1.0, 1.0, 0.5, 0.0, 150, -1.0},
        {"task03_m3_pid.csv", 3, 1.0, 2.0, 0.3, 1.0, 150, -1.0},
    };

    for (const DemoScenario& scenario : scenarios) {
        run_demo_scenario(scenario);
    }

    // Реакция объектов без регулятора: постоянное управление, при котором
    // y выходит на то же установившееся значение, что и под регулятором.
    struct OpenScenario {
        const char* csv;  ///< имя CSV-файла результата
        int plant;        ///< объект: 1 — 1.8, 2 — 2.2, 3 — 3.6
        double u;         ///< постоянное управляющее воздействие
        int steps;        ///< число тактов
    };

    const std::vector<OpenScenario> open_scenarios = {
        {"task03_open_linear.csv", 1, 18.75, 60},
        {"task03_open_m2.csv", 2, 0.3, 60},
        {"task03_open_m3.csv", 3, 0.2, 150},
    };

    std::cout << '\n';
    for (const OpenScenario& scenario : open_scenarios) {
        auto plant = make_demo_plant(scenario.plant);
        const std::vector<SimulationPoint> points =
            run_open_loop(*plant, scenario.u, scenario.steps);
        write_csv(scenario.csv, points);
        std::cout << scenario.csv << ": " << points.size()
                  << " тактов, y_end = " << points.back().y << '\n';
    }
}

int main(int argc, char* argv[]) {
    if (argc > 1 && std::string(argv[1]) == "--demo") {
        run_demo();
        return 0;
    }

    run_interactive();
    return 0;
}
