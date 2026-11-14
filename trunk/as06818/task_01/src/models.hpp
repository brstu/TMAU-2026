#ifndef MODELS_HPP
#define MODELS_HPP

#include <cmath>
#include <algorithm>
#include <vector>

// Типы входных воздействий
enum class InputType {
    Step,       
    Impulse,   
    Harmonic    
};

// Функция для генерации входа u_tau
inline double get_input(InputType type, int tau) {
    switch (type) {
        case InputType::Step:
            return 1.0;
        case InputType::Impulse:
            return (tau == 0) ? 1.0 : 0.0;
        case InputType::Harmonic:
            return std::sin(static_cast<double>(tau));
    }
    return 0.0;
}

//  один шаг симуляции
struct StepResult {
    int tau;
    double u;
    double y;
};

// базовый абстрактный класс
class Model {
public:
    virtual ~Model() = default;

    virtual double step(double u) = 0;

    virtual void reset() = 0;

    // Запуск полной симуляции
    std::vector<StepResult> simulate(int n, InputType input_type) {
        reset();
        std::vector<StepResult> results;
        results.reserve(n);

        for (int tau = 0; tau < n; ++tau) {
            double u = get_input(input_type, tau);
            double y = step(u);
            results.push_back({tau, u, y});
        }
        return results;
    }
};

class Model1_8 : public Model {
private:
    double a1, a2, b1, b2;
    double y_curr = 0.0;
    double y_prev = 0.0;
    double u_prev = 0.0;

public:
    Model1_8(double a1 = 0.5, double a2 = 0.2, double b1 = 0.3, double b2 = 0.1)
        : a1(a1), a2(a2), b1(b1), b2(b2) {}

    void reset() override {
        y_curr = 0.0;
        y_prev = 0.0;
        u_prev = 0.0;
    }

    double step(double u) override {
        double y_next = a1 * y_curr + a2 * y_prev + b1 * u + b2 * u_prev;
        
        // Сдвиг состояний
        y_prev = y_curr;
        y_curr = y_next;
        u_prev = u;

        return y_next;
    }
};


class Model2_2 : public Model {
private:
    double a, b;
    double u_min, u_max;
    double y_curr = 0.0;

    double sat(double u) const {
        return std::clamp(u, u_min, u_max);
    }

public:
    Model2_2(double a = 0.7, double b = 0.5, double u_min = -0.5, double u_max = 0.5)
        : a(a), b(b), u_min(u_min), u_max(u_max) {}

    void reset() override {
        y_curr = 0.0;
    }

    double step(double u) override {
        double y_next = a * y_curr + b * sat(u);
        y_curr = y_next;
        return y_next;
    }
};


class Model3_6 : public Model {
private:
    double a, b, dt;
    double y_curr = 0.0;

public:
    Model3_6(double a = -0.1, double b = 0.5, double dt = 0.1)
        : a(a), b(b), dt(dt) {}

    void reset() override {
        y_curr = 0.0;
    }

    double step(double u) override {
        double dy = a * std::pow(y_curr, 3) + b * u;
        y_curr += dy * dt;
        return y_curr;
    }
};

#endif