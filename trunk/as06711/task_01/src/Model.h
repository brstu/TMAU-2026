
#ifndef MODEL_BASE_H
#define MODEL_BASE_H


// Абстрактный базовый класс для всех моделей варианта.
// Хранит текущее значение выхода и задаёт общий интерфейс расчёта.
class Model {
public:
    virtual ~Model() = default;

    // Один шаг моделирования: получить вход u, вернуть новое y
    virtual double calculateNext(double u) = 0;

    // Привести внутреннее состояние к начальному
    virtual void reset() = 0;

    // Текущее значение выхода
    double getY() const { return y_curr; }

    // Принудительно установить выход (используется наследниками)
    void setY(double value) { y_curr = value; }

private: // <--- ИСПРАВЛЕНО: было protected, стало private
    // Значение выхода по умолчанию — нулевое начальное состояние
    double y_curr = 0.0;
};

#endif // MODEL_BASE_H