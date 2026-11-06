// Лабораторная работа №1. Моделирование объекта управления.
#include <cstdlib>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>
#include "simulator.hpp"
#ifdef _WIN32
#include <process.h>
#include <windows.h>
#else
#include <sys/wait.h>
#include <unistd.h>
#endif

namespace {

struct Options {
    std::string model = "1.1";
    std::string out = "output";
    std::vector<Input> inputs = {Input::Step};
    int n = 20;
    double y0 = 0;
    double amp = 1;
    bool quiet = false;
    bool plot = true;
    Params params;
};

enum class ParseResult { Ok, Help, Error };

void usage() {
    std::puts(
"Использование: sim [опции]\n"
"  Без опций программа запускается в интерактивном режиме и сама\n"
"  запрашивает модель, число шагов N и параметры модели.\n"
"\n"
"  --model ID      1.1..1.10, 2.1..2.10, 3.1..3.10 или all   (по умолчанию 1.1)\n"
"  --input TYPE    step | impulse | sine | all               (по умолчанию step)\n"
"  --n N           число шагов симуляции                     (по умолчанию 20)\n"
"  --y0 V          начальное значение y                      (по умолчанию 0)\n"
"  --amp V         амплитуда ступенчатого входа              (по умолчанию 1)\n"
"  --out DIR       каталог для csv                           (по умолчанию output)\n"
"  --quiet         не печатать таблицу в консоль\n"
"  --no-plot       не строить график после расчёта\n"
"  --<параметр> V  любой параметр модели: a b c d a1 a2 a3 b1 b2 b3 k Umax Umin delta eps dt");
}

bool toDouble(const std::string& text, double& value) {
    try {
        size_t pos = 0;
        value = std::stod(text, &pos);
        return pos == text.size();
    } catch (const std::invalid_argument&) {
        return false;
    } catch (const std::out_of_range&) {
        return false;
    }
}

bool toInt(const std::string& text, int& value) {
    try {
        size_t pos = 0;
        value = std::stoi(text, &pos);
        return pos == text.size();
    } catch (const std::invalid_argument&) {
        return false;
    } catch (const std::out_of_range&) {
        return false;
    }
}

bool parseInputs(std::string_view text, std::vector<Input>& inputs) {
    if (text == "step") inputs = {Input::Step};
    else if (text == "impulse") inputs = {Input::Impulse};
    else if (text == "sine") inputs = {Input::Sine};
    else if (text == "all") inputs = {Input::Step, Input::Impulse, Input::Sine};
    else return false;
    return true;
}

// Каталог для csv должен оставаться внутри текущего каталога:
// запрещаем абсолютные пути и переходы "..", чтобы нельзя было писать куда угодно.
bool isSafeRelativeDir(const std::string& dir) {
    if (dir.empty()) return false;
    const std::filesystem::path p(dir);
    if (p.is_absolute() || p.has_root_name() || p.has_root_directory()) return false;
    for (const auto& part : p) {
        if (part == "..") return false;
    }
    return true;
}

// Применяет одну пару "--ключ значение" к настройкам.
bool applyOption(Options& opt, const std::string& key, const std::string& value) {
    if (key == "model") { opt.model = value; return true; }
    if (key == "out") {
        if (!isSafeRelativeDir(value)) return false;
        opt.out = value;
        return true;
    }
    if (key == "input") return parseInputs(value, opt.inputs);
    if (key == "n") return toInt(value, opt.n) && opt.n >= 1;
    if (key == "y0") return toDouble(value, opt.y0);
    if (key == "amp") return toDouble(value, opt.amp);
    double v = 0;
    if (!toDouble(value, v)) return false;
    opt.params.v[key] = v;  // параметр модели (a, b, a1, k, dt, ...)
    return true;
}

ParseResult parseArgs(const std::vector<std::string>& args, Options& opt) {
    std::string pending;  // ключ вида "--n", ожидающий значения
    for (const auto& arg : args) {
        if (!pending.empty()) {
            if (!applyOption(opt, pending.substr(2), arg)) {
                std::cerr << "Неверное значение для " << pending << ": " << arg << "\n";
                return ParseResult::Error;
            }
            pending.clear();
        } else if (arg == "--help" || arg == "-h") {
            return ParseResult::Help;
        } else if (arg == "--quiet") {
            opt.quiet = true;
        } else if (arg == "--no-plot") {
            opt.plot = false;
        } else if (arg.rfind("--", 0) == 0) {
            pending = arg;
        } else {
            std::cerr << "Неверный аргумент: " << arg << "\n";
            return ParseResult::Error;
        }
    }
    if (!pending.empty()) {
        std::cerr << "Не указано значение для " << pending << "\n";
        return ParseResult::Error;
    }
    return ParseResult::Ok;
}

// ---------------------------------------------------------------- интерактив

// Признак того, что ввод закончился (EOF): дальше спрашивать бессмысленно.
bool g_inputClosed = false;

// Читает строку; пустая строка означает "оставить значение по умолчанию".
std::string ask(const std::string& prompt) {
    if (g_inputClosed) return {};
    std::cout << prompt << std::flush;
    std::string line;
    if (!std::getline(std::cin, line)) {
        g_inputClosed = true;
        return {};
    }
    return line;
}

// Компактная запись числа: 0.5 вместо 0.500000.
std::string fmt(double v) {
    std::ostringstream out;
    out << std::setprecision(6) << std::defaultfloat << v;
    return out.str();
}

double askDouble(const std::string& name, double def) {
    for (;;) {
        const std::string line = ask("  " + name + " [" + fmt(def) + "]: ");
        if (line.empty()) return def;
        if (double v = 0; toDouble(line, v)) return v;
        std::cout << "  Ожидается число, повторите ввод.\n";
    }
}

int askInt(const std::string& name, int def, int minValue) {
    for (;;) {
        const std::string line = ask("  " + name + " [" + std::to_string(def) + "]: ");
        if (line.empty()) return def;
        if (int v = 0; toInt(line, v) && v >= minValue) return v;
        std::cout << "  Ожидается целое число >= " << minValue << ", повторите ввод.\n";
    }
}

// Печатает модели одного блока (id начинается с prefix).
void printBlock(const std::string& prefix, const char* header) {
    std::cout << header << "\n";
    for (const auto& id : allIds()) {
        if (id.rfind(prefix, 0) != 0) continue;
        const auto m = makeModel(id, Params{});
        std::cout << "    " << std::left << std::setw(5) << id << ' ' << std::right
                  << (m ? m->title() : std::string()) << "\n";
    }
}

void printModelList() {
    std::cout << "\nДоступные модели:\n";
    printBlock("1.", "  Блок 1 — линейные дискретные модели:");
    printBlock("2.", "  Блок 2 — нелинейные дискретные модели:");
    printBlock("3.", "  Блок 3 — дифференциальные уравнения (метод Эйлера):");
}

// Спрашивает у пользователя модель, N, y0, амплитуду, вход и параметры модели.
void askInteractive(Options& opt) {
    printModelList();
    for (;;) {
        const std::string id = ask("\nВыберите модель (например 1.3): ");
        if (makeModel(id, Params{})) { opt.model = id; break; }
        if (g_inputClosed) {
            std::cout << "\nВвод завершён — используется модель " << opt.model << ".\n";
            break;
        }
        std::cout << "  Модели '" << id << "' нет в списке, повторите ввод.\n";
    }

    const auto probe = makeModel(opt.model, Params{});
    std::cout << "\nМодель " << probe->id() << ": " << probe->title() << "\n";
    std::puts("Параметры модели (Enter — оставить значение по умолчанию):");
    for (const auto& [name, def] : probe->params()) {
        opt.params.v[name] = askDouble(name, def);
    }

    std::puts("\nПараметры эксперимента:");
    opt.n = askInt("N (число шагов)", opt.n, 1);
    opt.y0 = askDouble("y0 (начальное значение)", opt.y0);
    opt.amp = askDouble("amp (амплитуда ступеньки)", opt.amp);
    bool ok = false;
    while (!ok) {
        const std::string line = ask("  вход (step/impulse/sine/all) [all]: ");
        ok = line.empty() ? parseInputs("all", opt.inputs) : parseInputs(line, opt.inputs);
        if (!ok) std::cout << "  Допустимо: step, impulse, sine, all.\n";
    }
}

// ---------------------------------------------------------------- графики

// Каталог, из которого запущена программа (для поиска plot.py).
std::filesystem::path exeDir(const char* argv0) {
    std::error_code ec;
    auto p = std::filesystem::absolute(argv0, ec);
    if (ec) return std::filesystem::current_path();
    return p.parent_path();
}

// Ищет plot.py рядом с исполняемым файлом и в родительских каталогах.
std::filesystem::path findPlotScript(const char* argv0) {
    std::error_code ec;
    std::filesystem::path dir = exeDir(argv0);
    for (int i = 0; i < 5; ++i) {
        if (const auto candidate = dir / "plot.py"; std::filesystem::is_regular_file(candidate, ec)) {
            return candidate;
        }
        const auto parent = dir.parent_path();
        if (parent == dir) break;  // достигли корня файловой системы
        dir = parent;
    }
    return {};
}

// Запускает процесс без командной оболочки: аргументы передаются массивом,
// поэтому имена файлов с пробелами и кавычками не могут внедрить команду.
// Возвращает true, если процесс завершился с кодом 0.
bool runProcess(const std::vector<std::string>& argv) {
    std::vector<const char*> raw;
    raw.reserve(argv.size() + 1);
    for (const auto& a : argv) raw.push_back(a.c_str());
    raw.push_back(nullptr);
#ifdef _WIN32
    const intptr_t rc = _spawnvp(_P_WAIT, raw[0], raw.data());
    return rc == 0;
#else
    const pid_t pid = fork();
    if (pid < 0) return false;
    if (pid == 0) {
        execvp(raw[0], const_cast<char* const*>(raw.data()));
        _exit(127);  // exec не удался
    }
    int status = 0;
    if (waitpid(pid, &status, 0) < 0) return false;
    return WIFEXITED(status) && WEXITSTATUS(status) == 0;
#endif
}

// Запускает python plot.py --show <csv...>, чтобы график появился сразу.
void showPlots(const char* argv0, const std::vector<std::string>& csvFiles) {
    if (csvFiles.empty()) return;
    const auto script = findPlotScript(argv0);
    if (script.empty()) {
        std::puts("\nplot.py не найден — график не построен (csv сохранены).");
        return;
    }
    std::vector<std::string> args = {script.string(), "--show"};
    args.insert(args.end(), csvFiles.begin(), csvFiles.end());
    std::cout << "\nПостроение графиков: python " << script.string() << "\n" << std::flush;
#ifdef _WIN32
    // Консоль переключена в UTF-8, поэтому и python должен писать в UTF-8.
    _putenv_s("PYTHONIOENCODING", "utf-8");
#endif
    // Сначала пробуем "python", затем лаунчер "py" (типично для Windows).
    std::vector<std::string> cmd = {"python"};
    cmd.insert(cmd.end(), args.begin(), args.end());
    if (runProcess(cmd)) return;
    cmd = {"py", "-3"};
    cmd.insert(cmd.end(), args.begin(), args.end());
    if (!runProcess(cmd)) {
        std::puts("Не удалось запустить python/matplotlib — график не показан (csv сохранены).");
    }
}

// ---------------------------------------------------------------- расчёт

bool runInput(const Model& model, const Options& opt, Input type,
              std::vector<std::string>& csvFiles) {
    std::cout << "  -- вход: " << inputName(type) << ", N = " << opt.n << "\n";
    const auto u = makeInput(type, opt.n, opt.amp);
    const auto y = simulate(model, u, opt.y0, opt.n);
    if (!opt.quiet) printTable(u, y);
    const std::string path = opt.out + "/model_" + model.id() + "_" + inputName(type) + ".csv";
    if (saveCsv(path, u, y)) {
        std::cout << "  CSV: " << path << "\n";
        csvFiles.push_back(path);
    }
    return true;
}

bool runModel(const std::string& id, const Options& opt, std::vector<std::string>& csvFiles) {
    const auto model = makeModel(id, opt.params);
    if (!model) {
        std::cerr << "Неизвестная модель: " << id << "\n";
        return false;
    }
    std::cout << "\n=== Модель " << model->id() << ": " << model->title() << " ===\n";
    std::string info;
    const bool stable = model->isStable(info);
    std::cout << "  Устойчивость: " << info << "\n";
    if (!stable) {
        std::cout << "  *** ПРЕДУПРЕЖДЕНИЕ: система НЕУСТОЙЧИВА (корень вне единичного круга)"
                     " — процесс будет расходиться! ***\n";
    }
    for (Input type : opt.inputs) runInput(*model, opt, type, csvFiles);
    return true;
}

}  // namespace

int main(int argc, char** argv) {
#ifdef _WIN32
    SetConsoleOutputCP(65001);
#endif
    Options opt;
    const std::vector<std::string> args(argv + 1, argv + argc);
    if (args.empty()) {
        askInteractive(opt);  // без аргументов — всё спрашиваем у пользователя
    } else {
        switch (parseArgs(args, opt)) {
            case ParseResult::Help: usage(); return 0;
            case ParseResult::Error: usage(); return 1;
            case ParseResult::Ok: break;
        }
    }

    std::error_code ec;
    std::filesystem::create_directories(opt.out, ec);
    if (ec) {
        std::cerr << "Не удалось создать каталог '" << opt.out << "': " << ec.message() << "\n";
        return 1;
    }

    std::vector<std::string> csvFiles;
    const std::vector<std::string> ids = opt.model == "all" ? allIds() : std::vector<std::string>{opt.model};
    for (const auto& id : ids) {
        if (!runModel(id, opt, csvFiles)) return 1;
    }
    if (opt.plot) showPlots(argv[0], csvFiles);
    return 0;
}
