#pragma once

#include <string>

// Общий интерфейс дискретной модели управляемого объекта (вариант 15).
// Наследники описывают конкретное уравнение и хранят свою историю состояний.
class Model {
public:
    Model() = default;
    Model(const Model&) = default;
    Model& operator=(const Model&) = default;
    virtual ~Model() = default;

    // Расчёт следующего шага симуляции по текущему входному воздействию
    virtual double calculateNext(double u) = 0;

    // Возврат модели в нулевое начальное состояние
    virtual void reset() = 0;

    // Анализ устойчивости. Текстовое пояснение записывается в report.
    virtual bool isStable(std::string& report) const {
        report = "No stability criterion for this model.";
        return true;
    }

    // Последнее рассчитанное значение выхода
    double output() const { return lastOutput_; }

protected:
    void storeOutput(double value) { lastOutput_ = value; }

private:
    double lastOutput_ = 0.0;
};
