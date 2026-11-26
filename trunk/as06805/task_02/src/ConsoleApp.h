#pragma once

#include <iosfwd>
#include <string>

// Параметры запуска интерактивного сценария.
struct AppOptions {
    // Имена файлов для экспорта результатов (пустая строка — не экспортировать).
    std::string csvFileName = "results.csv";
    std::string txtFileName = "results.txt";
};

// Интерактивный сценарий работы программы: выбор модели и сигнала,
// анализ устойчивости, симуляция и экспорт результатов.
// Ввод и вывод вынесены в потоки, поэтому сценарий полностью тестируем.
int runApplication(std::istream& in, std::ostream& out, const AppOptions& options = AppOptions());
