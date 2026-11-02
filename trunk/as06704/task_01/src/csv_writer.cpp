#include "csv_writer.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

namespace {

std::string escapeCsv(const std::string& value) {
    std::string escaped = "\"";
    for (char character : value) {
        if (character == '"') {
            escaped += "\"\"";
        } else {
            escaped += character;
        }
    }
    escaped += '"';
    return escaped;
}

}

bool CsvWriter::save(const std::string& filename,
                     const std::vector<Sample>& samples,
                     const std::string& modelName,
                     const std::string& signalName) {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Не удалось создать файл " << filename << ".\n";
        return false;
    }

    file << "tau,u_tau,y_tau,model,signal\n";
    file << std::setprecision(12);
    for (const Sample& sample : samples) {
        file << sample.tau << ','
             << sample.input << ','
             << sample.output << ','
             << escapeCsv(modelName) << ','
             << escapeCsv(signalName) << '\n';
    }

    if (!file.good()) {
        std::cerr << "Ошибка записи в файл " << filename << ".\n";
        return false;
    }

    return true;
}

void CsvWriter::printTable(const std::vector<Sample>& samples) {
    std::cout << "\nРезультаты моделирования\n";
    std::cout << std::fixed << std::setprecision(6);
    std::cout << "+--------+----------------+----------------+\n";
    std::cout << "| tau    | u_tau          | y_tau          |\n";
    std::cout << "+--------+----------------+----------------+\n";
    for (const Sample& sample : samples) {
        std::cout << "| " << std::setw(6) << sample.tau
                  << " | " << std::setw(14) << sample.input
                  << " | " << std::setw(14) << sample.output << " |\n";
    }
    std::cout << "+--------+----------------+----------------+\n";
}
