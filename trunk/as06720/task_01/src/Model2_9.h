#ifndef MODEL_2_9_H
#define MODEL_2_9_H

#include "Model.h"
#include <string>

// Model 2.9: управляющее воздействие модулируется квадратным корнем
// с сохранением знака. Формула модели приведена в doc/readme.md.
class Model2_9 : public Model {
public:
    Model2_9(double coeffA, double coeffB);

    double calculateNext(double input) override;
    void reset() override;

    // Однородная часть модели устойчива, если модуль коэффициента a меньше единицы
    bool isStable(std::string& message) const override;

private:
    static double sign(double value);

    double a;
    double b;
};

#endif // MODEL_2_9_H
