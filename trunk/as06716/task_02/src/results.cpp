#include "results.hpp"

#include <fstream>
#include <iomanip>
#include <ostream>

namespace {

constexpr int PRECISION = 6;

}  // namespace


void printTable(std::ostream& out, const std::string& model, const std::string& input, const std::vector<Sample>& samples) {
    const auto flags = out.flags();
    const auto precision = out.precision();
    out << std::fixed << std::setprecision(PRECISION);

    out << "\nМодель: " << model << ", воздействие: " << input << "\n"
        << std::setw(6) << "tau" << std::setw(14) << "u" << std::setw(14) << "y" << "\n";
    for (const Sample& s : samples) {
        out << std::setw(6) << s.tau << std::setw(14) << s.u << std::setw(14) << s.y << "\n";
    }

    out.flags(flags);
    out.precision(precision);
}

void writeCsv(std::ostream& out, const std::string& model, const std::string& input, const std::vector<Sample>& samples) {
    const auto flags = out.flags();
    const auto precision = out.precision();
    out << std::fixed << std::setprecision(PRECISION);

    out << "model,input,tau,u,y\n";
    for (const Sample& s : samples) {
        out << model << ',' << input << ',' << s.tau << ',' << s.u << ',' << s.y << '\n';
    }

    out.flags(flags);
    out.precision(precision);
}

bool saveCsv(const std::filesystem::path& file, const std::string& model, const std::string& input, const std::vector<Sample>& samples) {
    std::error_code error;
    if (file.has_parent_path()) {
        std::filesystem::create_directories(file.parent_path(), error);
    }

    std::ofstream csv(file);
    if (error || !csv) {
        return false;
    }
    writeCsv(csv, model, input, samples);

    return static_cast<bool>(csv);
}
