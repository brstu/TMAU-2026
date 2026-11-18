#include "ResultWriter.h"

#include <fstream>
#include <iomanip>

bool ResultWriter::saveCsv(const std::vector<ResultRow>& rows, const std::string& path) {
    std::ofstream file(path);
    if (!file) return false;

    file << "model,input,tau,u,y\n";
    file << std::setprecision(12);
    for (const auto& row : rows) {
        file << '"' << row.model << "\",\"" << row.input << "\"," << row.tau
             << ',' << row.u << ',' << row.y << '\n';
    }
    return static_cast<bool>(file);
}
