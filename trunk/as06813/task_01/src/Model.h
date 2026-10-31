#ifndef MODEL_H
#define MODEL_H

#include <string>

class Model {
protected:
    double y = 0.0;

public:
    virtual ~Model() = default;

    // Ввод коэффициентов модели с клавиатуры
    virtual void readParameters() = 0;

    virtual double nextStep(double u, double dt) = 0;

    virtual void reset() {
        y = 0.0;
    }

    // Шаг по времени: для дискретных моделей 1, для дифференциальной - dt
    virtual double timeStep() const {
        return 1.0;
    }

    virtual std::string name() const = 0;   // название для меню
    virtual std::string code() const = 0;   // номер модели: "1.3", "2.7", "3.1"

    double getOutput() const {
        return y;
    }
};

#endif
