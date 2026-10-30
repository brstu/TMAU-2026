#pragma once
// Базовые классы: контекст симуляции, параметры, абстрактная модель.
#include <cmath>
#include <map>
#include <string>
#include <vector>

// Параметры модели (a, b, a1, ... ) с значениями по умолчанию.
struct Params {
    std::map<std::string, double> v;
    double get(const std::string& k, double def) const {
        auto it = v.find(k);
        return it == v.end() ? def : it->second;
    }
};

// Контекст шага: история y и u до момента tau включительно.
// Y(k) = y_{tau-k}, U(k) = u_{tau-k}; до начала симуляции значения равны 0.
struct Ctx {
    const std::vector<double>& y;
    const std::vector<double>& u;
    int t;
    double Y(int k = 0) const { return t - k >= 0 ? y[t - k] : 0.0; }
    double U(int k = 0) const { return t - k >= 0 ? u[t - k] : 0.0; }
};

// Абстрактная модель объекта управления.
class Model {
public:
    Model(std::string id, std::string title) : id_(std::move(id)), title_(std::move(title)) {}
    virtual ~Model() = default;

    // Расчёт y_{tau+1} по истории (чисто виртуальный метод).
    virtual double nextStep(const Ctx& c) const = 0;

    // Проверка устойчивости. Для нелинейных моделей и ДУ по умолчанию не применима.
    virtual bool isStable(std::string& info) const {
        info = "проверка устойчивости не применима (модель нелинейная / ДУ)";
        return true;
    }
    const std::string& id() const { return id_; }
    const std::string& title() const { return title_; }

private:
    std::string id_;
    std::string title_;
};