#ifndef TASK03_AS06818_CONTROL_SYSTEM_HPP
#define TASK03_AS06818_CONTROL_SYSTEM_HPP

#include <vector>

#include "model.hpp"
#include "pid.hpp"

/// @file control_system.hpp
/// Замкнутая система регулирования и режимы симуляции.

/// Одна точка процесса регулирования (замкнутый контур).
struct ControllerPoint {
    int tau;   ///< номер такта
    double w;  ///< задающее воздействие
    double e;  ///< ошибка регулирования e = w - y
    double u;  ///< управляющее воздействие
    double y;  ///< выходная величина объекта
};

/// Одна точка процесса без регулятора (открытый контур).
struct SimulationPoint {
    int tau;   ///< номер такта
    double u;  ///< постоянное управляющее воздействие
    double y;  ///< выходная величина объекта
};

/// Замкнутая САУ: объект управления охвачен отрицательной обратной связью
/// через ПИД-регулятор. Выполняет по одному такту регулирования, поэтому
/// один и тот же объект можно вести любым режимом симуляции.
class ControlSystem {
public:
    /// Связывает объект plant с регулятором pid (ссылки должны
    /// существовать всё время жизни системы).
    ControlSystem(Model& plant, PidController& pid);

    /// Один такт: e = w - y; u = ПИД(e); y = объект(u).
    ControllerPoint step(double w);

    /// Сброс объекта, регулятора и счётчика тактов.
    void reset();

private:
    Model& plant;
    PidController& pid;
    double y = 0.0;
    int tau = 0;
};

/// Режим А: моделирование фиксированных n тактов.
std::vector<ControllerPoint> run_fixed_steps(ControlSystem& system, double w,
                                             int n);

/// Режим Б: моделирование до установившегося режима — пока |e| не станет
/// меньше eps на window тактов подряд. Ограничение max_steps защищает от
/// бесконечного цикла при расходящемся процессе.
std::vector<ControllerPoint> run_until_steady(ControlSystem& system, double w,
                                              double eps, int window = 3,
                                              int max_steps = 100000);

/// Прогон объекта без регулятора при постоянном управлении u
/// (для сравнения «до/после» на графиках).
std::vector<SimulationPoint> run_open_loop(Model& plant, double u, int n);

#endif
