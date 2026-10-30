#ifndef MODEL_1_5_H
#define MODEL_1_5_H

#include "Model.h"
#include <deque>
#include <string>

// Model 1.5 — линейная система с запаздыванием по состоянию:
//   y(t+1) = a1*y(t) + a2*y(t-k) + b*u(t),   k >= 1
class Model1_5 : public Model {
public:
    Model1_5(double coeffA1, double coeffA2, double coeffB, int delayK);

    double calculateNext(double input) override;
    void reset() override;

    // Анализ устойчивости: корни z^(k+1) - a1*z^k - a2 = 0 должны лежать
    // внутри единичной окружности |z| < 1
    bool isStable(std::string& message) const override;

private:
    double a1;
    double a2;
    double b;
    int k;
    // История выходов: history[0] = y(t), history[k] = y(t-k)
    std::deque<double> history;
};

#endif // MODEL_1_5_H
