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

// - Модели -

/**
 * @brief Абстрактный базовый класс для всех моделей.
 *
 * Каждая модель — это дискретная динамическая система с состоянием.
 * Жизненный цикл:
 *   1. Создать объект с параметрами.
 *   2. Вызвать reset() для обнуления состояния.
 *   3. Вызывать step(u) на каждом шаге.
 *
 * Позволяет очень просто расширять код
 */
class Model {
public:
    virtual ~Model() = default;

    /**
     * @brief Сбросить внутреннее состояние в начальное.
     *        После вызова модель ведёт себя так, будто её только что создали.
     */
    virtual void reset() = 0;

    /**
     * @brief Сделать один шаг симуляции.
     * @param u Входное воздействие на текущем шаге.
     * @return Выход модели после этого шага.
     */
    virtual double step(double u) = 0;

    /**
     * @brief Человекочитаемое имя модели (используется в CSV и логах).
     */
    virtual std::string name() const = 0;
};

// - =Model15= -

/**
 * @brief Модель 1.5: линейная рекуррентная с задержкой на @c k шагов.
 *
 * Уравнение:
 * @f[
 *   y_{t+1} = a_1 y_t + a_2 y_{t-k} + b u_t
 * @f]
 *
 * Начальное условие: @f$ y_0 = 0 @f$. Если @f$ t < k @f$, то
 * @f$ y_{t-k} @f$ считается равным 0.
 *
 * Внутренне состояние хранится в кольцевом буфере размера @c k+1
 * (а не в растущем векторе), поэтому память не растёт с числом шагов.
 */
class Model15 : public Model {
public:
    /**
     * @brief Конструктор.
     * @param a1 Коэффициент при текущем выходе @f$ y_t @f$.
     * @param a2 Коэффициент при задержанном выходе @f$ y_{t-k} @f$.
     * @param b  Коэффициент при входе @f$ u_t @f$.
     * @param k  Задержка в шагах (>= 0).
     * @throw std::invalid_argument если @p k отрицателен.
     */
    Model15(double a1, double a2, double b, int k)
    : a1_(a1), a2_(a2), b_(b), k_(k) {
        if (k_ < 0) {
            throw std::invalid_argument("Model15: k must be >= 0");
        }
        // Кольцевой буфер: ровно k+1 слотов, все нули.
        // Позиция 0 соответствует y_0 = 0.
        y_.assign(static_cast<std::size_t>(k_) + 1, 0.0);
    }

    /** @brief Сбросить историю и счётчик шагов. */
    void reset() override {
        std::fill(y_.begin(), y_.end(), 0.0);
        steps_ = 0;
    }

    /** @copydoc Model::step(double) */
    double step(double u) override {
        const std::size_t n          = y_.size();          // = k + 1
        const std::size_t currentPos = steps_ % n;         // где лежит y_t
        const std::size_t delayedPos = (steps_ + 1) % n;   // (t-k) mod n == (t+1) mod n

        const double y_t  = y_[currentPos];
        // Если шагов сделано ещё меньше k, то y_{t-k} ещё "не существует"
        // и по определению равно 0.
        const double y_tk = (steps_ >= static_cast<std::size_t>(k_))
        ? y_[delayedPos]
        : 0.0;

        const double y_next = a1_ * y_t + a2_ * y_tk + b_ * u;

        // Записываем y_{t+1} поверх устаревшего y_{t-k} (мы его только что прочли).
        y_[delayedPos] = y_next;
        ++steps_;

        return y_next;
    }

    /** @copydoc Model::name() const */
    std::string name() const override {
        return "Model 1.5";
    }

private:
    double              a1_;     //< Коэффициент a1.
    double              a2_;     //< Коэффициент a2.
    double              b_;      //< Коэффициент b.
    int                 k_;      //< Задержка (>= 0).
    std::vector<double> y_;      //< Кольцевой буфер размера k+1 вместо предыдущего съедания памяти как О(n)
    std::size_t         steps_ = 0; //< Сколько шагов уже сделано (t).
};

// - ==Model27==  -

/**
 * @brief Модель 2.7: релейная с гистерезисом в полосе @f$[-\varepsilon, \varepsilon]@f$.
 *
 * Уравнение:
 * @f[
 *   y_{t+1} = a y_t + b r_t
 * @f]
 * где @f$ r_t \in \{-1, +1\} @f$ — состояние реле, определяемое входом
 * @f$ u_t @f$ и знаком предыдущего выхода. В полосе @f$|u| \le \varepsilon @f$
 * реле остаётся в прежнем состоянии (гистерезис).
 *
 * Ветка @c r = 0 в коде фактически недостижима для конечных входов и
 * оставлена как защита от нечисловых значений / будущих правок.
 */
class Model27 : public Model {
public:
    /**
     * @brief Конструктор.
     * @param a   Коэффициент при предыдущем выходе.
     * @param b   Коэффициент при выходе реле.
     * @param eps Полуширина зоны гистерезиса (>= 0).
     * @throw std::invalid_argument если @p eps отрицателен.
     */
    Model27(double a, double b, double eps)
    : a_(a), b_(b), eps_(eps) {
        if (eps_ < 0.0) {
            throw std::invalid_argument("Model27: eps must be >= 0");
        }
    }

    /** @brief Сбросить предыдущий выход в 0. */
    void reset() override {
        y_prev_ = 0.0;
    }

    /** @copydoc Model::step(double) */
    double step(double u) override {
        // Локальные булевы переменные делают условие читаемым.
        const bool prevPositive    = y_prev_ > 0.0;   // гистерезис "залип" в плюсе
        const bool prevNonPositive = y_prev_ <= 0.0;  // гистерезис "залип" в минусе

        double r;
        if (u > eps_ || (u >= -eps_ && prevPositive)) {
            // Вход выше +eps, либо внутри полосы при положительном y_prev
            r = 1.0;
        } else if (u < -eps_ || (u <= eps_ && prevNonPositive)) {
            // Вход ниже -eps, либо внутри полосы при неположительном y_prev
            r = -1.0;
        } else {
            // Защитная ветка: для конечных u недостижима, оставлена как
            // индикатор инварианта (см. разбор логики выше).
            r = 0.0;
        }

        const double y_next = a_ * y_prev_ + b_ * r;
        y_prev_ = y_next;
        return y_next;
    }

    /** @copydoc Model::name() const */
    std::string name() const override {
        return "Model 2.7";
    }

private:
    double a_;      //< Коэффициент при предыдущем выходе.
    double b_;      //< Коэффициент при выходе реле.
    double eps_;    //< Полуширина зоны гистерезиса.
    double y_prev_ = 0.0; //< Предыдущий выход (== текущее состояние).
};

// - ===Model39=== -

/**
 * @brief Модель 3.9: нелинейный интегратор с насыщением входа.
 *
 * Уравнение (явный Эйлер):
 * @f[
 *   y_{t+1} = y_t + dt \cdot b \cdot \tanh(u_t)
 * @f]
 *
 * Сам выход не ограничивается, ограничивается только мгновенное
 * приращение.
 */
class Model39 : public Model {
public:
    /**
     * @brief Конструктор.
     * @param b  Коэффициент усиления интегратора.
     * @param dt Шаг интегрирования.
     */
    Model39(double b, double dt)
    : b_(b), dt_(dt) {}

    /** @brief Сбросить выход в 0. */
    void reset() override {
        y_ = 0.0;
    }

    /** @copydoc Model::step(double) */
    double step(double u) override {
        y_ += dt_ * b_ * std::tanh(u);
        return y_;
    }

    /** @copydoc Model::name() const */
    std::string name() const override {
        return "Model 3.9";
    }

private:
    double b_;       //< Коэффициент усиления.
    double dt_;      //< Шаг интегрирования.
    double y_ = 0.0; //< Текущее значение интегратора.
};

// -- Входные воздействия --

/**
 * @brief Тип входного воздействия.
 *
 * Значения упорядочены: при итерации по @c std::set<InputType> порядок
 * будет Step -> Impulse -> Harmonic.
 *
 * Как и с моделями, очень просто расширить набор входных воздействий
 */
enum class InputType { Step, Impulse, Harmonic };

/**
 * @brief Преобразовать тип входа в строку для CSV и логов.
 * @param t Тип входа.
 * @return Строковое имя ("step", "impulse", "harmonic").
 */
const char* inputName(InputType t) {
    switch (t) {
        case InputType::Step:     return "step";
        case InputType::Impulse:  return "impulse";
        case InputType::Harmonic: return "harmonic";
    }
    return "?";
}

/**
 * @brief Сгенерировать значение входа на шаге @p tau.
 * @param t   Тип воздействия.
 * @param tau Номер шага (0-based).
 * @return Значение входа @f$ u_{tau} @f$.
 *
 * - Step:     @f$ u \equiv 1 @f$.
 * - Impulse:  @f$ u_0 = 1 @f$, дальше 0.
 * - Harmonic: @f$ u_\tau = \sin(\tau) @f$.
 */
double genInput(InputType t, int tau) {
    switch (t) {
        case InputType::Step:     return 1.0;
        case InputType::Impulse:  return (tau == 0) ? 1.0 : 0.0;
        case InputType::Harmonic: return std::sin(static_cast<double>(tau));
    }
    return 0.0;
}

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
