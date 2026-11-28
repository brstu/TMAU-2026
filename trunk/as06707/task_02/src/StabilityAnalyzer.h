#pragma once
#include <string>

class StabilityAnalyzer {
public:
    // Проверка корня характеристического уравнения на Z-плоскости (|z| < 1)
    static bool isLinearModelStable(double a);
    
    // Возвращает текст предупреждения, если система неустойчива
    static std::string getWarningMessage(double a);
};