#include "Models.hpp"
#include "InputSignals.hpp"
#include "SimulationRunner.hpp"
#include "StabilityAnalyzer.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <limits>
#include <sstream>
#include <array>
#include <cstdlib>

#ifdef _WIN32
#  define NOMINMAX
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#endif

class SimulationApp {
public:
    void run() {
#ifdef _WIN32
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
#endif
        std::cout << "Lab 1, variant 6 (App version)\n";

        bool active = true;
        while (active) {
            displayMenu();
            std::string choice = getStringInput("Choice: ");
            if (choice == "q" || choice == "Q" || choice == "exit" || choice == "Quit") {
                active = false;
            }
            else if (choice == "0") {
                execModel16();
                execModel28();
                execModel310();
            }
            else if (choice == "1") {
                execModel16();
            }
            else if (choice == "2") {
                execModel28();
            }
            else if (choice == "3") {
                execModel310();
            }
            else {
                std::cout << "  Invalid choice.\n";
            }
        }
        std::cout << "\nBye.\n";
    }

private:
    void displayMenu() const {
        std::cout << "\n--- MENU ---\n"
            << "  1 - Model 1.6\n"
            << "  2 - Model 2.8\n"
            << "  3 - Model 3.10\n"
            << "  0 - all\n"
            << "  q - quit\n";
    }

    std::string getStringInput(const std::string& prompt) const {
        std::cout << prompt;
        std::string s;
        std::getline(std::cin, s);

        size_t first = s.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) return "";
        size_t last = s.find_last_not_of(" \t\r\n");
        return s.substr(first, last - first + 1);
    }

    int getIntInput(const std::string& prompt) const {
        int val = 0;
        while (true) {
            std::cout << prompt;
            if (std::cin >> val) {
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                return val;
            }
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "  Invalid integer, retry.\n";
        }
    }

    double getDoubleInput(const std::string& prompt) const {
        double val = 0.0;
        while (true) {
            std::cout << prompt;
            if (std::cin >> val) {
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                return val;
            }
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "  Invalid double, retry.\n";
        }
    }

    int getSignalChoice() const {
        int c = 0;
        while (c < 1 || c > 3) {
            c = getIntInput("Signal (1=step, 2=impulse, 3=harmonic): ");
        }
        return c;
    }

    void outputResults(const Trace& tr, const std::string& baseName, const std::string& sigId) {
        Runner::show(tr);
        std::string csvFile = "out_" + baseName + "_" + sigId + ".csv";
        Runner::dumpCsv(tr, csvFile);

        if (Runner::diverged(tr)) {
            std::cout << "WARNING: solution diverged (model unstable)\n";
        }
        tryPlot(csvFile);
    }

    void tryPlot(const std::string& csvName) {
        const std::array<std::string, 5> searchPaths = {
            "render_plot.py", "..\\render_plot.py", "../render_plot.py", "src\\render_plot.py", "..\\..\\render_plot.py"
        };
        std::string script = "";
        for (const auto& p : searchPaths) {
            if (std::ifstream(p).good()) {
                script = p;
                break;
            }
        }
        if (script.empty()) return;

        if (std::system("py -3 --version >nul 2>nul") != 0) return;

        std::string cmd = "py -3 \"" + script + "\" " + csvName + " >nul 2>nul";
        if (std::system(cmd.c_str()) != 0) return;

        std::string png = csvName.substr(0, csvName.rfind(".csv")) + ".png";
        if (std::ifstream(png).good()) {
            std::system(("start \"\" \"" + png + "\" >nul 2>nul").c_str());
        }
    }

    void execModel16() {
        std::cout << "\n[Model 1.6]\n";
        int n = getIntInput("Steps n: ");
        auto sig = makeSignal(getSignalChoice(), 1.0);

        double a1 = getDoubleInput("  a1 = ");
        double a2 = getDoubleInput("  a2 = ");
        double a3 = getDoubleInput("  a3 = ");
        double b = getDoubleInput("  b  = ");

        auto stab = stability::analyzeModel16(a1, a2, a3);
        std::cout << "Stability: " << stab.message << "\n";
        if (!stab.stable) {
            std::cout << "WARNING: system is divergent!\n";
        }

        Runner r(std::make_unique<ThirdOrderLinear>(a1, a2, a3, b), n);
        outputResults(r.execute(*sig), "model16", sig->id());
    }

    void execModel28() {
        std::cout << "\n[Model 2.8]\n";
        int n = getIntInput("Steps n: ");
        auto sig = makeSignal(getSignalChoice(), 1.0);

        double a = getDoubleInput("  a  = ");
        double b = getDoubleInput("  b  = ");
        double c = getDoubleInput("  c  = ");
        double y0 = getDoubleInput("  y0 = ");

        Runner r(std::make_unique<LogisticMap>(a, b, c, y0), n);
        outputResults(r.execute(*sig), "model28", sig->id());
    }

    void execModel310() {
        std::cout << "\n[Model 3.10]\n";
        int n = getIntInput("Steps n: ");
        auto sig = makeSignal(getSignalChoice(), 1.0);

        double a = getDoubleInput("  a  = ");
        double b = getDoubleInput("  b  = ");
        double dt = getDoubleInput("  dt = ");
        double y0 = getDoubleInput("  y0 = ");

        Runner r(std::make_unique<OffsetODE>(a, b, dt, y0), n);
        outputResults(r.execute(*sig), "model310", sig->id());
    }
};

int main() {
    SimulationApp app;
    app.run();
    return 0;
}