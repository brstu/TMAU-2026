#include "Models.hpp"
#include "InputSignals.hpp"
#include "SimulationRunner.hpp"

#include <array>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <limits>
#include <memory>
#include <sstream>
#include <string>

#ifdef _WIN32
#  define NOMINMAX
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#endif

namespace {

    double askDouble(const std::string& label) {
        double v = 0.0;
        for (;;) {
            std::cout << label;
            if (std::cin >> v) {
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                return v;
            }
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "  Invalid number, try again." << std::endl;
        }
    }

    int askInt(const std::string& label) {
        int v = 0;
        for (;;) {
            std::cout << label;
            if (std::cin >> v) {
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                return v;
            }
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "  Invalid number, try again." << std::endl;
        }
    }

    int askSignalChoice() {
        for (;;) {
            std::cout << "Signal (1=step, 2=impulse, 3=harmonic): ";
            int c = 0;
            if (std::cin >> c) {
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                if (c >= 1 && c <= 3) return c;
            }
            else {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
            std::cout << "  Enter 1, 2 or 3." << std::endl;
        }
    }

    int askModel() {
        for (;;) {
            std::cout << std::endl;
            std::cout << "--- MENU ---" << std::endl;
            std::cout << "  1 - Model 1.6" << std::endl;
            std::cout << "  2 - Model 2.8" << std::endl;
            std::cout << "  3 - Model 3.10" << std::endl;
            std::cout << "  0 - all" << std::endl;
            std::cout << "  q - quit" << std::endl;
            std::cout << "Choice: ";

            std::string line;
            if (!std::getline(std::cin, line)) return -1;

            const size_t b = line.find_first_not_of(" \t\r\n");
            const size_t e = line.find_last_not_of(" \t\r\n");
            if (b == std::string::npos) {
                std::cout << "  Empty, try again." << std::endl;
                continue;
            }
            const std::string tok = line.substr(b, e - b + 1);

            if (tok == "q" || tok == "Q" ||
                tok == "quit" || tok == "Quit" ||
                tok == "exit" || tok == "Exit") {
                return -1;
            }

            try {
                const int v = std::stoi(tok);
                if (v == 0) return 0;
                if (v >= 1 && v <= 3) return v;
            }
            catch (...) {}

            std::cout << "  Invalid choice, try again." << std::endl;
        }
    }

    std::string findRenderScript() {
        const std::array<std::string, 6> cs = {
            "render_plot.py",
            "..\\render_plot.py",
            "../render_plot.py",
            "..\\..\\render_plot.py",
            "../../render_plot.py",
            "src\\render_plot.py"
        };
        for (const auto& c : cs) {
            std::ifstream f(c);
            if (f.good()) return c;
        }
        return "";
    }

    void invokePlot(const std::string& csvName) {
        const std::string script = findRenderScript();
        if (script.empty()) {
            return;
        }

        if (std::system("py -3 --version >nul 2>nul") != 0) {
            std::cout << "WARNING: 'py -3' not available "
                "(install Python with 'Add to PATH')" << std::endl;
            return;
        }

        const std::string cmd =
            "py -3 \"" + script + "\" " + csvName + " >nul 2>nul";
        if (std::system(cmd.c_str()) != 0) {
            std::cout << "WARNING: plot failed "
                "(py -3 -m pip install matplotlib)" << std::endl;
            return;
        }

        std::string png = csvName;
        const size_t dot = png.rfind(".csv");
        if (dot != std::string::npos) {
            png = png.substr(0, dot) + ".png";
        }

        std::string toOpen;
        std::ifstream probe(png);
        if (probe.good()) {
            toOpen = png;
        }
        else {
            const std::string stem = png.substr(0, png.rfind(".png"));
            for (int i = 1; i < 10; ++i) {
                const std::string cand = stem + "_" + std::to_string(i) + ".png";
                std::ifstream c(cand);
                if (c.good()) { toOpen = cand; break; }
            }
        }

        if (!toOpen.empty()) {
            const std::string openCmd =
                "start \"\" \"" + toOpen + "\" >nul 2>nul";
            std::system(openCmd.c_str());
        }
    }

    void finish(const Trace& tr, const std::string& csvName) {
        Runner::show(tr);
        Runner::dumpCsv(tr, csvName);

        if (Runner::diverged(tr)) {
            std::cout << "WARNING: solution diverged "
                "(model unstable)" << std::endl;
        }

        invokePlot(csvName);
    }

    void flowThirdOrder() {
        std::cout << "\n--- Model 1.6 ---" << std::endl;
        const int steps = askInt("Steps n: ");
        const int sigChoice = askSignalChoice();
        auto sig = makeSignal(sigChoice, 1.0);

        const double a1 = askDouble("  a1 = ");
        const double a2 = askDouble("  a2 = ");
        const double a3 = askDouble("  a3 = ");
        const double b = askDouble("  b  = ");

        Runner r(std::make_unique<ThirdOrderLinear>(a1, a2, a3, b), steps);
        const Trace tr = r.execute(*sig);
        finish(tr, std::string("out_model16_") + sig->id() + ".csv");
    }

    void flowLogistic() {
        std::cout << "\n--- Model 2.8 ---" << std::endl;
        const int steps = askInt("Steps n: ");
        const int sigChoice = askSignalChoice();
        auto sig = makeSignal(sigChoice, 1.0);

        const double a = askDouble("  a  = ");
        const double b = askDouble("  b  = ");
        const double c = askDouble("  c  = ");
        const double y0 = askDouble("  y0 = ");

        Runner r(std::make_unique<LogisticMap>(a, b, c, y0), steps);
        const Trace tr = r.execute(*sig);
        finish(tr, std::string("out_model28_") + sig->id() + ".csv");
    }

    void flowODE() {
        std::cout << "\n--- Model 3.10 ---" << std::endl;
        const int steps = askInt("Steps n: ");
        const int sigChoice = askSignalChoice();
        auto sig = makeSignal(sigChoice, 1.0);

        const double a = askDouble("  a  = ");
        const double b = askDouble("  b  = ");
        const double dt = askDouble("  dt = ");
        const double y0 = askDouble("  y0 = ");

        Runner r(std::make_unique<OffsetODE>(a, b, dt, y0), steps);
        const Trace tr = r.execute(*sig);
        finish(tr, std::string("out_model310_") + sig->id() + ".csv");
    }

}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    std::cout << "Lab 1, variant 6" << std::endl;

    for (;;) {
        const int choice = askModel();
        if (choice == -1) break;

        if (choice == 0) {
            flowThirdOrder();
            flowLogistic();
            flowODE();
        }
        else if (choice == 1) {
            flowThirdOrder();
        }
        else if (choice == 2) {
            flowLogistic();
        }
        else if (choice == 3) {
            flowODE();
        }
    }

    std::cout << "\nBye." << std::endl;
    return 0;
}