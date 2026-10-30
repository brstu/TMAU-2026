// Лабораторная работа №1. Моделирование объекта управления.
#include <cstdio>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>
#include "simulator.hpp"
#ifdef _WIN32
#include <windows.h>
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
    Params params;
};

enum class ParseResult { Ok, Help, Error };

void usage() {
    std::puts(
"Использование: sim [опции]\n"
"  --model ID      1.1..1.10, 2.1..2.10, 3.1..3.10 или all   (по умолчанию 1.1)\n"
"  --input TYPE    step | impulse | sine | all               (по умолчанию step)\n"
"  --n N           число шагов симуляции                     (по умолчанию 20)\n"
"  --y0 V          начальное значение y                      (по умолчанию 0)\n"
"  --amp V         амплитуда ступенчатого входа              (по умолчанию 1)\n"
"  --out DIR       каталог для csv                           (по умолчанию output)\n"
"  --quiet         не печатать таблицу в консоль\n"
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

// Применяет одну пару "--ключ значение" к настройкам.
bool applyOption(Options& opt, const std::string& key, const std::string& value) {
    if (key == "model") { opt.model = value; return true; }
    if (key == "out") { opt.out = value; return true; }
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

bool runInput(const Model& model, const Options& opt, Input type) {
    std::printf("  -- вход: %s, n = %d\n", inputName(type), opt.n);
    const auto u = makeInput(type, opt.n, opt.amp);
    const auto y = simulate(model, u, opt.y0, opt.n);
    if (!opt.quiet) printTable(u, y);
    if (const std::string path = opt.out + "/model_" + model.id() + "_" + inputName(type) + ".csv";
        saveCsv(path, u, y)) {
        std::printf("  CSV: %s\n", path.c_str());
    }
    return true;
}

bool runModel(const std::string& id, const Options& opt) {
    const auto model = makeModel(id, opt.params);
    if (!model) {
        std::cerr << "Неизвестная модель: " << id << "\n";
        return false;
    }
    std::printf("\n=== Модель %s: %s ===\n", model->id().c_str(), model->title().c_str());
    std::string info;
    const bool stable = model->isStable(info);
    std::printf("  Устойчивость: %s\n", info.c_str());
    if (!stable) {
        std::printf("  *** ПРЕДУПРЕЖДЕНИЕ: система НЕУСТОЙЧИВА (корень вне единичного круга) — процесс будет расходиться! ***\n");
    }
    for (Input type : opt.inputs) runInput(*model, opt, type);
    return true;
}

}  // namespace

int main(int argc, char** argv) {
#ifdef _WIN32
    SetConsoleOutputCP(65001);
#endif
    Options opt;
    switch (const std::vector<std::string> args(argv + 1, argv + argc); parseArgs(args, opt)) {
        case ParseResult::Help: usage(); return 0;
        case ParseResult::Error: usage(); return 1;
        case ParseResult::Ok: break;
    }

    std::error_code ec;
    std::filesystem::create_directories(opt.out, ec);
    if (ec) {
        std::cerr << "Не удалось создать каталог '" << opt.out << "': " << ec.message() << "\n";
        return 1;
    }

    const std::vector<std::string> ids = opt.model == "all" ? allIds() : std::vector<std::string>{opt.model};
    for (const auto& id : ids) {
        if (!runModel(id, opt)) return 1;
    }
    return 0;
}