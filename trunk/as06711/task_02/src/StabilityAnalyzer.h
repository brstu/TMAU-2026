#pragma once
#include <complex>
#include <cmath>
#include <iostream>
#include <ostream>
#include <string>

// Анализ устойчивости линейной модели 1.9 (вариант 19).
// Характеристическое уравнение:
//     z^3 - a1*z^2 - a2*z - a3 = 0
// Система устойчива, когда все три корня лежат строго внутри единичной окружности.
class StabilityAnalyzer {
public:
    struct Roots {
        std::complex<double> z1, z2, z3;
    };

    // Корни характеристического уравнения (формула Кардано + уточнение Ньютоном).
    static Roots cubicRoots(double a1, double a2, double a3) {
        const std::complex<double> A(-a1, 0.0);
        const std::complex<double> B(-a2, 0.0);
        const std::complex<double> C(-a3, 0.0);

        // Приводим кубическое уравнение к форме t^3 + p*t + q = 0, z = t - A/3.
        const std::complex<double> p = B - A * A / 3.0;
        const std::complex<double> q = (2.0 * A * A * A) / 27.0 - (A * B) / 3.0 + C;
        const std::complex<double> disc = q * q / 4.0 + p * p * p / 27.0;

        std::complex<double> u = std::pow(-q / 2.0 + std::sqrt(disc), 1.0 / 3.0);
        Roots roots{};
        if (std::abs(u) < 1e-12) {
            // Тройной корень: p = q = 0.
            const std::complex<double> z = -A / 3.0;
            roots = {z, z, z};
        } else {
            const std::complex<double> omega(-0.5, std::sqrt(3.0) / 2.0);
            const std::complex<double> omega2 = omega * omega;
            roots.z1 = u            - p / (3.0 * u)            - A / 3.0;
            roots.z2 = omega  * u   - p / (3.0 * omega  * u)   - A / 3.0;
            roots.z3 = omega2 * u   - p / (3.0 * omega2 * u)   - A / 3.0;
        }

        roots.z1 = refine(roots.z1, A, B, C);
        roots.z2 = refine(roots.z2, A, B, C);
        roots.z3 = refine(roots.z3, A, B, C);
        return roots;
    }

    // Критерий устойчивости по корням Z-плоскости.
    static bool isStable(double a1, double a2, double a3) {
        const Roots r = cubicRoots(a1, a2, a3);
        const double tol = 1e-9; // граница |z| = 1 считается неустойчивой
        return std::abs(r.z1) < 1.0 - tol &&
               std::abs(r.z2) < 1.0 - tol &&
               std::abs(r.z3) < 1.0 - tol;
    }

    // Аналитическая проверка (критерий Жури для кубического полинома)
    // используется в тестах как независимый эталон.
    static bool isStableJury(double a1, double a2, double a3) {
        return std::abs(a3) < 1.0 &&
               1.0 - a1 - a2 - a3 > 0.0 &&
               1.0 + a1 - a2 + a3 > 0.0 &&
               1.0 + a2 + a1 * a3 - a3 * a3 > 0.0;
    }

    // Предупреждение в консоль перед началом симуляции.
    static bool warnIfUnstable(double a1, double a2, double a3,
                               std::ostream& out = std::cout) {
        if (isStable(a1, a2, a3)) {
            return true;
        }
        const Roots r = cubicRoots(a1, a2, a3);
        out << "[WARNING] Unstable process! Roots of z^3 - a1*z^2 - a2*z - a3 = 0:\n"
            << "          z1 = " << r.z1 << ", z2 = " << r.z2 << ", z3 = " << r.z3 << "\n"
            << "          At least one root lies on/outside the unit circle (|z| >= 1).\n";
        return false;
    }

private:
    static std::complex<double> refine(std::complex<double> z,
                                       const std::complex<double>& A,
                                       const std::complex<double>& B,
                                       const std::complex<double>& C) {
        for (int i = 0; i < 30; ++i) {
            const std::complex<double> f  = ((z + A) * z + B) * z + C;
            const std::complex<double> df = (3.0 * z + 2.0 * A) * z + B;
            if (std::abs(df) < 1e-14) break;
            const std::complex<double> dz = f / df;
            z -= dz;
            if (std::abs(dz) < 1e-15) break;
        }
        return z;
    }
};
