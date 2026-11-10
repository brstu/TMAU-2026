#ifndef STABILITY_ANALYZER_H
#define STABILITY_ANALYZER_H

#include <vector>
#include <complex>

class StabilityAnalyzer {
public:
    // Находит корни характеристического уравнения дискретной модели 1.4: z - a = 0 -> z = a
    static std::vector<std::complex<double>> getRootsLinearModel1_4(double a);

    // Проверяет, лежат ли все корни строго внутри единичного круга на Z-плоскости (|z| < 1)
    static bool isStableLinearModel1_4(double a);

    // Выполняет логическую проверку и выводит предупреждение в консоль при неустойчивости
    static bool checkAndWarnStability(double a);
};

#endif // STABILITY_ANALYZER_H