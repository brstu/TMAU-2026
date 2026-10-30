// Лабораторная работа №1. Моделирование объекта управления.
#include <cstdlib>
#include <cstring>
#include <iostream>
#include "simulator.hpp"
#ifdef _WIN32
#include <windows.h>
#endif

static void usage() {
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

int main(int argc, char** argv) {
#ifdef _WIN32
    SetConsoleOutputCP(65001);
#endif
    std::string model = "1.1", out = "output";
    std::vector<Input> inputs = {Input::Step};
    int n = 20; double y0 = 0, amp = 1; bool quiet = false;
    Params P;
    for (int i = 1; i < argc; ++i) {
        std::string k = argv[i];
        if (k == "--help" || k == "-h") { usage(); return 0; }
        if (k == "--quiet") { quiet = true; continue; }
        if (k.rfind("--", 0) != 0 || i + 1 >= argc) { std::cerr << "Неверный аргумент: " << k << "\n"; usage(); return 1; }
        std::string v = argv[++i]; k = k.substr(2);
        if (k == "model") model = v;
        else if (k == "input") {
            if (v == "step") inputs = {Input::Step};
            else if (v == "impulse") inputs = {Input::Impulse};
            else if (v == "sine") inputs = {Input::Sine};
            else if (v == "all") inputs = {Input::Step, Input::Impulse, Input::Sine};
            else { std::cerr << "Неизвестный тип входа: " << v << "\n"; return 1; }
        }
        else if (k == "n") n = std::atoi(v.c_str());
        else if (k == "y0") y0 = std::atof(v.c_str());
        else if (k == "amp") amp = std::atof(v.c_str());
        else if (k == "out") out = v;
        else P.v[k] = std::atof(v.c_str());
    }
    if (n < 1) { std::cerr << "n должно быть >= 1\n"; return 1; }
    std::vector<std::string> ids = model == "all" ? allIds() : std::vector<std::string>{model};
#ifdef _WIN32
    std::system(("mkdir \"" + out + "\" 2>nul").c_str());
#else
    std::system(("mkdir -p \"" + out + "\"").c_str());
#endif
    for (const auto& id : ids) {
        auto m = makeModel(id, P);
        if (!m) { std::cerr << "Неизвестная модель: " << id << "\n"; return 1; }
        std::printf("\n=== Модель %s: %s ===\n", m->id().c_str(), m->title().c_str());
        std::string info;
        bool stable = m->isStable(info);
        std::printf("  Устойчивость: %s\n", info.c_str());
        if (!stable)
            std::printf("  *** ПРЕДУПРЕЖДЕНИЕ: система НЕУСТОЙЧИВА (корень вне единичного круга) — процесс будет расходиться! ***\n");
        for (Input it : inputs) {
            std::printf("  -- вход: %s, n = %d\n", inputName(it), n);
            auto u = makeInput(it, n, amp);
            auto y = simulate(*m, u, y0, n);
            if (!quiet) printTable(u, y);
            std::string path = out + "/model_" + id + "_" + inputName(it) + ".csv";
            if (saveCsv(path, u, y)) std::printf("  CSV: %s\n", path.c_str());
        }
    }
    return 0;
}
