#pragma once
#include <vector>
#include <string>
#include <cmath>
#include <stdexcept>

// - Модели -

/**
 * @brief Абстрактный базовый класс для всех моделей.
 *
 * Каждая модель — это дискретная динамическая система с состоянием.
 * Жизненный цикл:
 *   1. Создать объект с параметрами.
 *   2. Вызвать reset() для обнуления состояния.
 *   3. Вызывать step(u) на каждом шаге.
 *
 * Позволяет очень просто расширять код
 */
class Model {
public:
    virtual ~Model() = default;

    /**
     * @brief Сбросить внутреннее состояние в начальное.
     *        После вызова модель ведёт себя так, будто её только что создали.
     */
    virtual void reset() = 0;

    /**
     * @brief Сделать один шаг симуляции.
     * @param u Входное воздействие на текущем шаге.
     * @return Выход модели после этого шага.
     */
    virtual double step(double u) = 0;

    /**
     * @brief Человекочитаемое имя модели (используется в CSV и логах).
     */
    virtual std::string name() const = 0;
};

// - =Model15= -

/**
 * @brief Модель 1.5: линейная рекуррентная с задержкой на @c k шагов.
 *
 * Уравнение:
 * @f[
 *   y_{t+1} = a_1 y_t + a_2 y_{t-k} + b u_t
 * @f]
 *
 * Начальное условие: @f$ y_0 = 0 @f$. Если @f$ t < k @f$, то
 * @f$ y_{t-k} @f$ считается равным 0.
 *
 * Внутренне состояние хранится в кольцевом буфере размера @c k+1
 * (а не в растущем векторе), поэтому память не растёт с числом шагов.
 */
class Model15 : public Model {
public:
    /**
     * @brief Конструктор.
     * @param a1 Коэффициент при текущем выходе @f$ y_t @f$.
     * @param a2 Коэффициент при задержанном выходе @f$ y_{t-k} @f$.
     * @param b  Коэффициент при входе @f$ u_t @f$.
     * @param k  Задержка в шагах (>= 0).
     * @throw std::invalid_argument если @p k отрицателен.
     */
    Model15(double a1, double a2, double b, int k)
    : a1_(a1), a2_(a2), b_(b), k_(k) {
        if (k_ < 0) {
            throw std::invalid_argument("Model15: k must be >= 0");
        }
        // Кольцевой буфер: ровно k+1 слотов, все нули.
        // Позиция 0 соответствует y_0 = 0.
        y_.assign(static_cast<std::size_t>(k_) + 1, 0.0);
    }

    /** @brief Сбросить историю и счётчик шагов. */
    void reset() override {
        std::fill(y_.begin(), y_.end(), 0.0);
        steps_ = 0;
    }

    /** @copydoc Model::step(double) */
    double step(double u) override {
        const std::size_t n          = y_.size();          // = k + 1
        const std::size_t currentPos = steps_ % n;         // где лежит y_t
        const std::size_t delayedPos = (steps_ + 1) % n;   // (t-k) mod n == (t+1) mod n

        const double y_t  = y_[currentPos];
        // Если шагов сделано ещё меньше k, то y_{t-k} ещё "не существует"
        // и по определению равно 0.
        const double y_tk = (steps_ >= static_cast<std::size_t>(k_))
        ? y_[delayedPos]
        : 0.0;

        const double y_next = a1_ * y_t + a2_ * y_tk + b_ * u;

        // Записываем y_{t+1} поверх устаревшего y_{t-k} (мы его только что прочли).
        y_[delayedPos] = y_next;
        ++steps_;

        return y_next;
    }

    /** @copydoc Model::name() const */
    std::string name() const override {
        return "Model 1.5";
    }

private:
    double              a1_;     //< Коэффициент a1.
    double              a2_;     //< Коэффициент a2.
    double              b_;      //< Коэффициент b.
    int                 k_;      //< Задержка (>= 0).
    std::vector<double> y_;      //< Кольцевой буфер размера k+1 вместо предыдущего съедания памяти как О(n)
    std::size_t         steps_ = 0; //< Сколько шагов уже сделано (t).
};

// - ==Model27==  -

/**
 * @brief Модель 2.7: релейная с гистерезисом в полосе @f$[-\varepsilon, \varepsilon]@f$.
 *
 * Уравнение:
 * @f[
 *   y_{t+1} = a y_t + b r_t
 * @f]
 * где @f$ r_t \in \{-1, +1\} @f$ — состояние реле, определяемое входом
 * @f$ u_t @f$ и знаком предыдущего выхода. В полосе @f$|u| \le \varepsilon @f$
 * реле остаётся в прежнем состоянии (гистерезис).
 *
 * Ветка @c r = 0 в коде фактически недостижима для конечных входов и
 * оставлена как защита от нечисловых значений / будущих правок.
 */
class Model27 : public Model {
public:
    /**
     * @brief Конструктор.
     * @param a   Коэффициент при предыдущем выходе.
     * @param b   Коэффициент при выходе реле.
     * @param eps Полуширина зоны гистерезиса (>= 0).
     * @throw std::invalid_argument если @p eps отрицателен.
     */
    Model27(double a, double b, double eps)
    : a_(a), b_(b), eps_(eps) {
        if (eps_ < 0.0) {
            throw std::invalid_argument("Model27: eps must be >= 0");
        }
    }

    /** @brief Сбросить предыдущий выход в 0. */
    void reset() override {
        y_prev_ = 0.0;
    }

    /** @copydoc Model::step(double) */
    double step(double u) override {
        // Локальные булевы переменные делают условие читаемым.
        const bool prevPositive    = y_prev_ > 0.0;   // гистерезис "залип" в плюсе
        const bool prevNonPositive = y_prev_ <= 0.0;  // гистерезис "залип" в минусе

        double r;
        if (u > eps_ || (u >= -eps_ && prevPositive)) {
            // Вход выше +eps, либо внутри полосы при положительном y_prev
            r = 1.0;
        } else if (u < -eps_ || (u <= eps_ && prevNonPositive)) {
            // Вход ниже -eps, либо внутри полосы при неположительном y_prev
            r = -1.0;
        } else {
            // Защитная ветка: для конечных u недостижима, оставлена как
            // индикатор инварианта (см. разбор логики выше).
            r = 0.0;
        }

        const double y_next = a_ * y_prev_ + b_ * r;
        y_prev_ = y_next;
        return y_next;
    }

    /** @copydoc Model::name() const */
    std::string name() const override {
        return "Model 2.7";
    }

private:
    double a_;      //< Коэффициент при предыдущем выходе.
    double b_;      //< Коэффициент при выходе реле.
    double eps_;    //< Полуширина зоны гистерезиса.
    double y_prev_ = 0.0; //< Предыдущий выход (== текущее состояние).
};

// - ===Model39=== -

/**
 * @brief Модель 3.9: нелинейный интегратор с насыщением входа.
 *
 * Уравнение (явный Эйлер):
 * @f[
 *   y_{t+1} = y_t + dt \cdot b \cdot \tanh(u_t)
 * @f]
 *
 * Сам выход не ограничивается, ограничивается только мгновенное
 * приращение.
 */
class Model39 : public Model {
public:
    /**
     * @brief Конструктор.
     * @param b  Коэффициент усиления интегратора.
     * @param dt Шаг интегрирования.
     */
    Model39(double b, double dt)
    : b_(b), dt_(dt) {}

    /** @brief Сбросить выход в 0. */
    void reset() override {
        y_ = 0.0;
    }

    /** @copydoc Model::step(double) */
    double step(double u) override {
        y_ += dt_ * b_ * std::tanh(u);
        return y_;
    }

    /** @copydoc Model::name() const */
    std::string name() const override {
        return "Model 3.9";
    }

private:
    double b_;       //< Коэффициент усиления.
    double dt_;      //< Шаг интегрирования.
    double y_ = 0.0; //< Текущее значение интегратора.
};

// -- Входные воздействия --

/**
 * @brief Тип входного воздействия.
 *
 * Значения упорядочены: при итерации по @c std::set<InputType> порядок
 * будет Step -> Impulse -> Harmonic.
 *
 * Как и с моделями, очень просто расширить набор входных воздействий
 */
enum class InputType { Step, Impulse, Harmonic };

/**
 * @brief Преобразовать тип входа в строку для CSV и логов.
 * @param t Тип входа.
 * @return Строковое имя ("step", "impulse", "harmonic").
 */
const char* inputName(InputType t) {
    switch (t) {
        case InputType::Step:     return "step";
        case InputType::Impulse:  return "impulse";
        case InputType::Harmonic: return "harmonic";
    }
    return "?";
}

/**
 * @brief Сгенерировать значение входа на шаге @p tau.
 * @param t   Тип воздействия.
 * @param tau Номер шага (0-based).
 * @return Значение входа @f$ u_{tau} @f$.
 *
 * - Step:     @f$ u \equiv 1 @f$.
 * - Impulse:  @f$ u_0 = 1 @f$, дальше 0.
 * - Harmonic: @f$ u_\tau = \sin(\tau) @f$.
 */
double genInput(InputType t, int tau) {
    switch (t) {
        case InputType::Step:     return 1.0;
        case InputType::Impulse:  return (tau == 0) ? 1.0 : 0.0;
        case InputType::Harmonic: return std::sin(static_cast<double>(tau));
    }
    return 0.0;
}
