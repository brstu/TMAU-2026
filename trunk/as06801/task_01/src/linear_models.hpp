#pragma once
// Блок 1. Линейные дискретные модели.
// y_{t+1} = sum_j yc[j]*y_{t-j} + sum_j uc[j]*u_{t-j}
#include <complex>
#include <cstdio>
#include "model.hpp"

class LinearModel : public Model {
public:
    LinearModel(const std::string& id, const std::string& title) : Model(id, title) {}

    double nextStep(const Ctx& c) const override {
        double r = 0;
        for (size_t j = 0; j < yc.size(); ++j) r += yc[j] * c.Y((int)j);
        for (size_t j = 0; j < uc.size(); ++j) r += uc[j] * c.U((int)j);
        return r;
    }

    // Характеристическое уравнение: z^m - yc0*z^(m-1) - ... - yc_{m-1} = 0.
    // Устойчивость: все корни внутри единичной окружности |z| < 1.
    bool isStable(std::string& info) const override {
        typedef std::complex<double> C;
        const int m = (int)yc.size();
        std::vector<C> p(m + 1);  // p[0]=1 (старший), p[j] = -yc[j-1]
        p[0] = 1;
        for (int j = 1; j <= m; ++j) p[j] = -yc[j - 1];
        std::vector<C> z(m);
        C seed(0.4, 0.9);
        for (int i = 0; i < m; ++i) z[i] = std::pow(seed, i);
        for (int it = 0; it < 2000; ++it) {  // метод Дюрана–Кернера
            double delta = 0;
            for (int i = 0; i < m; ++i) {
                C val = p[0];
                for (int j = 1; j <= m; ++j) val = val * z[i] + p[j];
                C den = 1;
                for (int j = 0; j < m; ++j) if (j != i) den *= (z[i] - z[j]);
                if (std::abs(den) < 1e-14) den = 1e-14;
                C d = val / den;
                z[i] -= d;
                delta = std::max(delta, std::abs(d));
            }
            if (delta < 1e-13) break;
        }
        double rmax = 0;
        char buf[256];
        info = "корни характеристического уравнения: ";
        for (int i = 0; i < m; ++i) {
            std::snprintf(buf, sizeof buf, "z%d = %.4f%+.4fi (|z|=%.4f)%s", i + 1,
                          z[i].real(), z[i].imag(), std::abs(z[i]), i + 1 < m ? "; " : "");
            info += buf;
            rmax = std::max(rmax, std::abs(z[i]));
        }
        return rmax < 1.0 - 1e-9;
    }

protected:
    std::vector<double> yc, uc;
};

// 1.1  y+ = a y + b u
struct Model11 : LinearModel {
    Model11(const Params& P) : LinearModel("1.1", "Инерционное звено 1-го порядка") {
        yc = {P.get("a", .8)}; uc = {P.get("b", .5)}; }
};
// 1.2  y+ = a y + b u_{t-k}
struct Model12 : LinearModel {
    Model12(const Params& P) : LinearModel("1.2", "Чистое запаздывание по управлению") {
        int k = (int)P.get("k", 2); if (k < 1) k = 1;
        yc = {P.get("a", .8)}; uc.assign(k + 1, 0.0); uc[k] = P.get("b", .5); }
};
// 1.3  y+ = a1 y + a2 y_{t-1} + b u
struct Model13 : LinearModel {
    Model13(const Params& P) : LinearModel("1.3", "Система 2-го порядка") {
        yc = {P.get("a1", .5), P.get("a2", .2)}; uc = {P.get("b", .5)}; }
};
// 1.4  y+ = a y + b (u - u_{t-1})
struct Model14 : LinearModel {
    Model14(const Params& P) : LinearModel("1.4", "Интегратор с потерями") {
        double b = P.get("b", .5);
        yc = {P.get("a", .8)}; uc = {b, -b}; }
};
// 1.5  y+ = a1 y + a2 y_{t-k} + b u
struct Model15 : LinearModel {
    Model15(const Params& P) : LinearModel("1.5", "Запаздывание по состоянию") {
        int k = (int)P.get("k", 2); if (k < 1) k = 1;
        yc.assign(k + 1, 0.0); yc[0] = P.get("a1", .5); yc[k] += P.get("a2", .2);
        uc = {P.get("b", .5)}; }
};
// 1.6  y+ = a1 y + a2 y_{t-1} + a3 y_{t-2} + b u
struct Model16 : LinearModel {
    Model16(const Params& P) : LinearModel("1.6", "Система 3-го порядка") {
        yc = {P.get("a1", .5), P.get("a2", .2), P.get("a3", .1)}; uc = {P.get("b", .5)}; }
};
// 1.7  y+ = a y + b1 u + b2 u_{t-1} + b3 u_{t-2}
struct Model17 : LinearModel {
    Model17(const Params& P) : LinearModel("1.7", "История управления (3 шага)") {
        yc = {P.get("a", .8)};
        uc = {P.get("b1", .5), P.get("b2", .3), P.get("b3", .2)}; }
};
// 1.8  y+ = a1 y + a2 y_{t-1} + b1 u + b2 u_{t-1}
struct Model18 : LinearModel {
    Model18(const Params& P) : LinearModel("1.8", "Обобщённая авторегрессионная модель") {
        yc = {P.get("a1", .5), P.get("a2", .2)}; uc = {P.get("b1", .5), P.get("b2", .3)}; }
};
// 1.9  y+ = a1 y + a2 y_{t-1} + a3 y_{t-2} + b1 u
struct Model19 : LinearModel {
    Model19(const Params& P) : LinearModel("1.9", "Трёхшаговая модель состояния") {
        yc = {P.get("a1", .5), P.get("a2", .2), P.get("a3", .1)}; uc = {P.get("b1", .5)}; }
};
// 1.10  y+ = a1 y + b1 u + b2 u_{t-2}
struct Model110 : LinearModel {
    Model110(const Params& P) : LinearModel("1.10", "Распределённое окно управления") {
        yc = {P.get("a1", .8)}; uc = {P.get("b1", .5), 0.0, P.get("b2", .3)}; }
};
