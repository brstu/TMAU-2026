#ifndef MODEL_1_5_H
#define MODEL_1_5_H

#include "Model.h"
#include <deque>
#include <string>

// Model 1.5: линейная система с запаздыванием по состоянию на k шагов.
// Формула модели приведена в doc/readme.md.
class Model1_5 : public Model {
public:
    Model1_5(double coeffA1, double coeffA2, double coeffB, int delayK);

    double calculateNext(double input) override;
    void reset() override;

    // Устойчивость: все корни характеристического уравнения
    // должны лежать внутри единичной окружности на Z-плоскости
    bool isStable(std::string& message) const override;

private:
    double a1;
    double a2;
    double b;
    int k;
    // Буфер последних k+1 значений выхода, от нового к старому
    std::deque<double> history;
};

#endif // MODEL_1_5_H
