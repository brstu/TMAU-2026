#ifndef MODEL_3_3_H
#define MODEL_3_3_H

#include "Model.h"
#include <string>

// Model 3.3: стандартный процесс первого порядка.
// Дифференциальное уравнение решается явным методом Эйлера с шагом dt.
// Формулы модели приведены в doc/readme.md.
class Model3_3 : public Model {
public:
    Model3_3(double coeffA, double coeffB, double timeStep);

    double calculateNext(double input) override;
    void reset() override;

    // Разностная схема устойчива, если её корень лежит внутри единичной окружности
    bool isStable(std::string& message) const override;

private:
    double a;
    double b;
    double dt;
};

#endif // MODEL_3_3_H
