#ifndef MODEL_BASE_H
#define MODEL_BASE_H

#include <string>

// Абстрактный базовый класс для всех моделей варианта 15.
// Хранит текущее значение выхода и задаёт общий интерфейс расчёта.
class Model {
public:
    virtual ~Model() = default;

    // Один шаг моделирования: принимает текущий вход, возвращает новое значение выхода
    virtual double calculateNext(double u) = 0;

    // Привести внутреннее состояние к начальному
    virtual void reset() = 0;

    // Проверка устойчивости (по умолчанию модель считается устойчивой).
    // Возвращает true, если процесс сходится, а в message записывает пояснение.
    virtual bool isStable(std::string& message) const {
        message = "Stability analysis is not defined for this model.";
        return true;
    }

    // Текущее значение выхода
    double getY() const { return y_curr; }

protected:
    // Установить выход (используется наследниками)
    void setY(double value) { y_curr = value; }

private:
    // Нулевое начальное состояние
    double y_curr = 0.0;
};

#endif // MODEL_BASE_H
