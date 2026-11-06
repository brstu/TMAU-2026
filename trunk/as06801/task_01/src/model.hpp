#pragma once
// Базовые классы: контекст симуляции, параметры, абстрактная модель.
#include <cmath>
#include <functional>
#include <map>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

// Параметры модели (a, b, a1, ... ) с значениями по умолчанию.
struct Params {
    std::map<std::string, double, std::less<>> v;
    double get(std::string_view k, double def) const {
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

    // Параметры, которые использует модель: (имя, значение по умолчанию).
    // Заполняется автоматически при вызове param() в конструкторе наследника.
    const std::vector<std::pair<std::string, double>>& params() const { return params_; }

protected:
    // Читает параметр из P (или значение по умолчанию) и запоминает его,
    // чтобы программа могла запросить это значение у пользователя.
    double param(const Params& P, std::string name, double def) {
        // Порядок не должен зависеть от порядка вычисления аргументов setCoefficients,
        // поэтому список параметров держим отсортированным по имени.
        auto pos = params_.begin();
        while (pos != params_.end() && pos->first < name) ++pos;
        params_.insert(pos, {name, def});
        return P.get(name, def);
    }

private:
    std::string id_;
    std::string title_;
    std::vector<std::pair<std::string, double>> params_;
};
