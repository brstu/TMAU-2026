#pragma once
#include <cstddef>
#include <fstream>
#include <string>
#include <vector>

// Мини-утилита для сохранения траектории моделирования в CSV.
// Вынесена в отдельный заголовок, чтобы логика записи не размазывалась
// по main.cpp и легко переиспользовалась в новых отчётах.
namespace io {

inline void writeCsv(const std::string& path,
                     const std::string& columnName,
                     const std::vector<double>& values) {
    std::ofstream out(path);
    if (!out) {
        return;
    }
    out << "step," << columnName << '\n';
    for (std::size_t i = 0; i < values.size(); ++i) {
        out << i << ',' << values[i] << '\n';
    }
}

} // namespace io