/**
 * Заметка, Alt+shif+D делают Doxygen-коммменты
 * @file main.cpp
 * @brief Симулятор дискретных моделей 1.5, 2.7 и 3.9.
 *
 * Программа принимает параметры из командной строки, прогоняет выбранные
 * модели на выбранных входных воздействиях и пишет результат в CSV.
 *
 * Структура CSV:
 * @code
 *   model,input,step,u,y
 * @endcode
 * где @c step - номер шага, начиная с 1.
 *
 * Пример запуска:
 * @code
 *   ./app --models 15,27 --input step,impulse --n 30 --out result.csv
 * @endcode
 */
#include "models.h"
#include "stability_analyzer.h"

#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <string>
#include <string_view>
#include <set>
#include <sstream>
#include <cmath>
#include <memory>
#include <algorithm>
#include <functional>
#include <map>
#include <cstdlib>
#include <cstddef>
#include <exception>
#include <stdexcept>



// --- Конфигурация ---

/**
 * @brief Параметры запуска симуляции.
 *
 * Значения по умолчанию соответствуют запуску "всё на всём":
 * все модели, все входы, 50 шагов, файл output.csv.
 */
struct Config {
    /// Множество выбранных моделей ("15", "27", "39").
    std::set<std::string, std::less<>> models = {"15", "27", "39"};

    int n = 50; ///< Количество шагов симуляции.

    /// Множество выбранных входных воздействий.
    std::set<InputType> inputs = {
        InputType::Step,
        InputType::Impulse,
        InputType::Harmonic,
    };

    std::string outFile = "output.csv"; //< Путь к выходному CSV.

    // Параметры Model 1.5
    double m15_a1 = 0.6; //< Коэффициент a1.
    double m15_a2 = 0.2; //< Коэффициент a2.
    double m15_b  = 0.5; //< Коэффициент b.
    int    m15_k  = 2;   //< Задержка (>= 0).

    // Параметры Model 2.7
    double m27_a   = 0.7; //< Коэффициент a.
    double m27_b   = 0.4; //< Коэффициент b.
    double m27_eps = 0.1; //< Полуширина гистерезиса (>= 0).

    // Параметры Model 3.9
    double m39_b  = 1.0; //< Коэффициент b.
    double m39_dt = 0.1; //< Шаг интегрирования.
};

// ---- Справка ----

/**
 * @brief Напечатать справку по командной строке в stdout.
 */
static void printHelp() {
    std::cout <<
    R"(Usage: app [options]

Common:
  --models 15,27,39                  list of models to run (default: all)
  --n 50                             number of simulation steps
  --input step,impulse,harmonic      comma-separated list (default: all)
  --out output.csv                   output CSV path

Model 1.5 parameters:
  --m15-a1  0.6
  --m15-a2  0.2
  --m15-b   0.5
  --m15-k   2

Model 2.7 parameters:
  --m27-a   0.7
  --m27-b   0.4
  --m27-eps 0.1

Model 3.9 parameters:
  --m39-b   1.0
  --m39-dt  0.1

  --help  -h                         show this help
)";
}

// ----- Разбор аргументов -----

/**
 * @brief Разобрать список моделей вида "15,27,39". (и тп при добавлении оных)
 *
 * Пробелы вокруг элементов обрезаются; неизвестные имена отвергаются.
 *
 * @param c Конфигурация, куда записать результат (поле @c models
 *          предварительно очищается).
 * @param v Строка-значение аргумента @c --models.
 */
static void parseModelsArg(Config& c, std::string_view v) {
    c.models.clear();

    std::stringstream ss{std::string(v)};
    std::string tok;

    while (std::getline(ss, tok, ',')) {
        // Обрезаем пробелы/табы вокруг токена.
        const auto first = tok.find_first_not_of(" \t");
        const auto last  = tok.find_last_not_of(" \t");
        if (first == std::string::npos) {
            // Пустой токен - пропускаем (например, лишняя запятая).
            continue;
        }
        tok = tok.substr(first, last - first + 1);

        if (tok != "15" && tok != "27" && tok != "39") {
            std::cerr << "Unknown model: " << tok << "\n";
            std::exit(1);
        }

        c.models.insert(tok);
    }

    if (c.models.empty()) {
        std::cerr << "--models list is empty\n";
        std::exit(1);
    }
}

/**
 * @brief Разобрать аргументы командной строки в @ref Config.
 *
 * Поддерживает @c --help, @c --models, @c --input и таблицу
 * параметров отдельных моделей. При ошибке печатает сообщение
 * в stderr и завершает программу с кодом 1.
 *
 * @param argc Аргумент main.
 * @param argv Аргумент main.
 * @return Заполненный @ref Config.
 */
static Config parseArgs(int argc, char** argv) {
    Config c;

    // Таблица сеттеров для "простых" параметров (кроме списков).
    // Ключ - имя флага, значение - лямбда, присваивающая значение.
    using Setter = std::function<void(Config&, std::string_view)>;

    static const std::map<std::string, Setter, std::less<>> setters = {
        {"--n",       [](Config& c, std::string_view v){ c.n        = std::stoi(std::string(v)); }},
        {"--out",     [](Config& c, std::string_view v){ c.outFile  = std::string(v); }},
        {"--m15-a1",  [](Config& c, std::string_view v){ c.m15_a1   = std::stod(std::string(v)); }},
        {"--m15-a2",  [](Config& c, std::string_view v){ c.m15_a2   = std::stod(std::string(v)); }},
        {"--m15-b",   [](Config& c, std::string_view v){ c.m15_b    = std::stod(std::string(v)); }},
        {"--m15-k",   [](Config& c, std::string_view v){
            c.m15_k = std::stoi(std::string(v));
            if (c.m15_k < 0) {
                throw std::invalid_argument("--m15-k must be >= 0");
            }
        }},
        {"--m27-a",   [](Config& c, std::string_view v){ c.m27_a    = std::stod(std::string(v)); }},
        {"--m27-b",   [](Config& c, std::string_view v){ c.m27_b    = std::stod(std::string(v)); }},
        {"--m27-eps", [](Config& c, std::string_view v){
            c.m27_eps = std::stod(std::string(v));
            if (c.m27_eps < 0.0) {
                throw std::invalid_argument("--m27-eps must be >= 0");
            }
        }},
        {"--m39-b",   [](Config& c, std::string_view v){ c.m39_b    = std::stod(std::string(v)); }},
        {"--m39-dt",  [](Config& c, std::string_view v){ c.m39_dt   = std::stod(std::string(v)); }},
    };

    int i = 1;
    while (i < argc) {
        const std::string_view a = argv[i];

        if (a == "--help" || a == "-h") {
            printHelp();
            std::exit(0);
        }

        if (i + 1 >= argc) {
            std::cerr << "Missing value after " << a << "\n";
            std::exit(1);
        }

        const std::string_view v = argv[i + 1];

        try {
            if (a == "--models") {
                parseModelsArg(c, v);
            } else if (a == "--input") {
                // Разбираем список входов через запятую.
                c.inputs.clear();

                std::stringstream ss{std::string(v)};
                std::string tok;

                while (std::getline(ss, tok, ',')) {
                    const auto first = tok.find_first_not_of(" \t");
                    const auto last  = tok.find_last_not_of(" \t");
                    if (first == std::string::npos) {
                        continue;
                    }
                    tok = tok.substr(first, last - first + 1);

                    if (tok == "all") {
                        c.inputs.insert(InputType::Step);
                        c.inputs.insert(InputType::Impulse);
                        c.inputs.insert(InputType::Harmonic);
                    } else if (tok == "step") {
                        c.inputs.insert(InputType::Step);
                    } else if (tok == "impulse") {
                        c.inputs.insert(InputType::Impulse);
                    } else if (tok == "harmonic") {
                        c.inputs.insert(InputType::Harmonic);
                    } else {
                        std::cerr << "Unknown --input value: " << tok << "\n";
                        std::exit(1);
                    }
                }

                if (c.inputs.empty()) {
                    std::cerr << "--input list is empty\n";
                    std::exit(1);
                }
            } else {
                const auto it = setters.find(a);
                if (it == setters.end()) {
                    std::cerr << "Unknown argument: " << a << "\n";
                    std::exit(1);
                }
                it->second(c, v);
            }
        } catch (const std::exception& e) {
            std::cerr << "Bad value for " << a << ": " << e.what() << "\n";
            std::exit(1);
        }

        i += 2;
    }

    if (c.n <= 0) {
        std::cerr << "--n must be > 0\n";
        std::exit(1);
    }

    return c;
}

// ----- main ------

/**
 * @brief Точка входа.
 *
 * Разбирает аргументы, прогоняет выбранные модели на выбранных входах
 * (декартово произведение), пишет CSV и краткую сводку в stderr.
 */
int main(int argc, char** argv) {
    Config cfg = parseArgs(argc, argv);

    std::ofstream fout(cfg.outFile);
    if (!fout) {
        std::cerr << "Cannot open " << cfg.outFile << "\n";
        return 1;
    }

    // Заголовок: колонка "step" содержит номер шага, начиная с 1
    // (значение tau + 1). Раньше заголовок назывался "tau", что
    // расходилось с содержимым.
    fout << "model,input,step,u,y\n";
    fout << std::fixed << std::setprecision(6);

    // Декартово произведение: для каждого входа - независимый прогон
    // всех выбранных моделей. Модели не связаны друг с другом.
    for (InputType inputType : cfg.inputs) {
        std::vector<std::unique_ptr<Model>> models;

        if (cfg.models.count("15")) {
            models.push_back(std::make_unique<Model15>(
                cfg.m15_a1, cfg.m15_a2, cfg.m15_b, cfg.m15_k));
        }
        if (cfg.models.count("27")) {
            models.push_back(std::make_unique<Model27>(
                cfg.m27_a, cfg.m27_b, cfg.m27_eps));
        }
        if (cfg.models.count("39")) {
            models.push_back(std::make_unique<Model39>(
                cfg.m39_b, cfg.m39_dt));
        }

        for (const auto& m : models) {
            m->reset();

            for (int tau = 0; tau < cfg.n; ++tau) {
                const double u = genInput(inputType, tau);
                const double y = m->step(u);

                fout << m->name() << ","
                << inputName(inputType) << ","
                << (tau + 1) << ","
                << u << ","
                << y << "\n";
            }
        }
    }

    // Краткая сводка в stderr (stdout оставлен свободным).
    std::cerr << "Done. Models:";
    for (const auto& m : cfg.models) {
        std::cerr << " " << m;
    }
    std::cerr << " | n=" << cfg.n
    << " | inputs=";
    bool first = true;
    for (InputType t : cfg.inputs) {
        if (!first) {
            std::cerr << ",";
        }
        std::cerr << inputName(t);
        first = false;
    }
    std::cerr << " | out=" << cfg.outFile << "\n";

    return 0;
}
