#include "Simulator.h"

namespace {

// Ширина строки в символах UTF-8 (по кодовым точкам, а не по байтам).
size_t utf8Width(const std::string& text) {
    size_t width = 0;
    for (unsigned char c : text) {
        if ((c & 0xC0) != 0x80) {
            ++width;
        }
    }
    return width;
}

// Выравнивание строки по правому краю до заданной ширины в символах.
std::string padRight(const std::string& text, size_t width) {
    const size_t len = utf8Width(text);
    return (len >= width) ? text : text + std::string(width - len, ' ');
}

}  // namespace

std::vector<double> Simulator::run(const ISignalSource& signal) {
    if (!targetModel_) {
        return {};
    }

    targetModel_->reset();
    std::vector<double> outputHistory;
    outputHistory.reserve(static_cast<size_t>(totalSteps_ > 0 ? totalSteps_ : 0));

    for (int stepIdx = 0; stepIdx < totalSteps_; ++stepIdx) {
        double currentInput = signal.get(static_cast<double>(stepIdx));
        double currentOutput = targetModel_->step(currentInput);
        outputHistory.push_back(currentOutput);
    }

    return outputHistory;
}

void Simulator::printTable(const std::vector<double>& results, const ISignalSource& signal,
                           std::ostream& out) const {
    out << "\n=========================================\n";
    out << " Модель: " << modelName() << "\n";
    out << "=========================================\n";
    out << padRight("Шаг (tau)", 12)
        << padRight("Вход u(t)", 14)
        << padRight("Выход y(t)", 16) << "\n";
    out << "-----------------------------------------\n";

    for (size_t i = 0; i < results.size(); ++i) {
        const double uVal = signal.get(static_cast<double>(i));
        out << std::setw(12) << i
            << std::setw(14) << std::fixed << std::setprecision(4) << uVal
            << std::setw(16) << std::fixed << std::setprecision(4) << results[i] << "\n";
    }
    out << "-----------------------------------------\n";
}

void Simulator::exportCSV(const std::vector<double>& results, const ISignalSource& signal,
                          const std::string& filePath) const {
    std::ofstream fileOut(filePath);
    if (!fileOut.is_open()) {
        std::cerr << "Ошибка: Не удалось открыть файл " << filePath << " для записи!\n";
        return;
    }

    fileOut << "tau,u,y\n";
    for (size_t i = 0; i < results.size(); ++i) {
        double uVal = signal.get(static_cast<double>(i));
        fileOut << i << "," << uVal << "," << results[i] << "\n";
    }
}

std::string Simulator::modelName() const {
    return targetModel_ ? targetModel_->name() : std::string("N/A");
}
