#pragma once
#include <vector>

class Model {
public:
    virtual ~Model() = default;
    // следующий шаг модели
    virtual double step(double u) = 0;

    virtual void reset() = 0;

    virtual const char* name() const = 0;
    // установка начальных условий
    virtual void setInitialState(const std::vector<double>& init) {
    // По умолчанию ничего не делает — переопределяется наследниками,
    // если у модели есть начальные условия.
    (void)init;
}
};
