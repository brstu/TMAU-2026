#pragma once
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

namespace io {

inline void writeCsv(const std::string& path,
                     const std::string& columnName,
                     const std::vector<double>& values) {
    // Защита от Path Traversal (SonarCloud S2083)
    std::filesystem::path target(path);
    std::filesystem::path cwd = std::filesystem::current_path();
    std::filesystem::path resolved = std::filesystem::weakly_canonical(target);
    
    if (resolved.string().rfind(cwd.string(), 0) != 0) {
        throw std::runtime_error("Path traversal detected: " + path);
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