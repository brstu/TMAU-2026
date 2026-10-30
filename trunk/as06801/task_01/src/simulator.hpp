#pragma once
// Входные воздействия, фабрика моделей, симуляция, вывод.
#include <algorithm>
#include <cstdio>
#include <memory>
#include <string_view>
#include <vector>
#include "linear_models.hpp"
#include "nonlinear_models.hpp"
#include "ode_models.hpp"

enum class Input { Step, Impulse, Sine };

inline const char* inputName(Input i) {
    switch (i) {
        case Input::Step: return "step";
        case Input::Impulse: return "impulse";
        default: return "sine";
    }
}
// Значение входного воздействия в момент tau.
inline double inputValue(Input t, int tau, double amp) {
    switch (t) {
        case Input::Step: return amp;
        case Input::Impulse: return tau == 0 ? 1.0 : 0.0;
        default: return std::sin(static_cast<double>(tau));
    }
}

// u_tau для tau = 0..n
inline std::vector<double> makeInput(Input t, int n, double amp) {
    std::vector<double> u(n + 1);
    for (int tau = 0; tau <= n; ++tau) u[tau] = inputValue(t, tau, amp);
    return u;
}

inline std::vector<std::string> allIds() {
    return {"1.1","1.2","1.3","1.4","1.5","1.6","1.7","1.8","1.9","1.10",
            "2.1","2.2","2.3","2.4","2.5","2.6","2.7","2.8","2.9","2.10",
            "3.1","3.2","3.3","3.4","3.5","3.6","3.7","3.8","3.9","3.10"};
}

inline std::unique_ptr<Model> makeModel(std::string_view id, const Params& P) {
#define M(ID, CLS) if (id == ID) return std::unique_ptr<Model>(new CLS(P));
    M("1.1",Model11) M("1.2",Model12) M("1.3",Model13) M("1.4",Model14) M("1.5",Model15)
    M("1.6",Model16) M("1.7",Model17) M("1.8",Model18) M("1.9",Model19) M("1.10",Model110)
    M("2.1",Model21) M("2.2",Model22) M("2.3",Model23) M("2.4",Model24) M("2.5",Model25)
    M("2.6",Model26) M("2.7",Model27) M("2.8",Model28) M("2.9",Model29) M("2.10",Model210)
    M("3.1",Model31) M("3.2",Model32) M("3.3",Model33) M("3.4",Model34) M("3.5",Model35)
    M("3.6",Model36) M("3.7",Model37) M("3.8",Model38) M("3.9",Model39) M("3.10",Model310)
#undef M
    return nullptr;
}

// Симуляция: y_0 = y0, далее y_{tau+1} = model.nextStep(...), tau = 0..n-1.
inline std::vector<double> simulate(const Model& m, const std::vector<double>& u, double y0, int n) {
    std::vector y(static_cast<size_t>(n) + 1, 0.0);
    y[0] = y0;
    for (int t = 0; t < n; ++t) {
        y[t + 1] = m.nextStep(Ctx{y, u, t});
        if (!std::isfinite(y[t + 1])) {
            std::printf("  ! Расходимость: y стало нечисловым на шаге tau=%d, расчёт остановлен\n", t + 1);
            y.resize(t + 1);
            break;
        }
    }
    return y;
}

inline void printTable(const std::vector<double>& u, const std::vector<double>& y) {
    std::printf("  %6s | %12s | %14s\n", "tau", "u", "y");
    std::printf("  -------+--------------+---------------\n");
    for (size_t t = 0; t < y.size(); ++t) std::printf("  %6zu | %12.5f | %14.6g\n", t, u[t], y[t]);
}

inline bool saveCsv(const std::string& path, const std::vector<double>& u, const std::vector<double>& y) {
    FILE* f = std::fopen(path.c_str(), "w");
    if (!f) return false;
    std::fprintf(f, "tau,u,y\n");
    for (size_t t = 0; t < y.size(); ++t) std::fprintf(f, "%zu,%.8g,%.8g\n", t, u[t], y[t]);
    std::fclose(f);
    return true;
}