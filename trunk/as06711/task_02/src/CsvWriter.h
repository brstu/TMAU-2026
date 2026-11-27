#pragma once
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace io {

// Специализированное исключение для защиты от Path Traversal
class PathTraversalException : public std::runtime_error {
public:
    // Наследуем конструкторы базового класса std::runtime_error (решение для SonarCloud)
    using std::runtime_error::runtime_error;
};

inline void writeCsv(const std::string& path,
                     const std::string& columnName,
                     const std::vector<double>& values) {
    // Защита от Path Traversal (SonarCloud S2083)
    std::filesystem::path target(path);
    std::filesystem::path cwd = std::filesystem::current_path();
    std::filesystem::path resolved = std::filesystem::weakly_canonical(target);
    
    if (resolved.string().rfind(cwd.string(), 0) != 0) {
        throw PathTraversalException("Path traversal detected: " + path);
    }

    std::ofstream out(resolved);
    if (!out) {
        return;
    }
    out << "step," << columnName << '\n';
    for (std::size_t i = 0; i < values.size(); ++i) {
        out << i << ',' << values[i] << '\n';
    }
}

} // namespace io