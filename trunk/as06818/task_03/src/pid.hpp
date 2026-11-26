#ifndef TASK03_AS06818_PID_HPP
#define TASK03_AS06818_PID_HPP

/// @file pid.hpp
/// Дискретный ПИД-регулятор в инкрементной форме.

/// Дискретный ПИД-регулятор (инкрементный алгоритм):
///   du[t] = Kp*(e[t] - e[t-1]) + Ki*e[t]*dt + Kd/dt*(e[t] - 2*e[t-1] + e[t-2])
///   u[t]  = u[t-1] + du[t]
/// Управляющий сигнал ограничивается диапазоном [u_min, u_max] —
/// пределами исполнительного устройства; накопленное за время насыщения
/// «залипание» (integral windup) при этом не возникает, так как история
/// хранит уже ограниченное значение u[t-1].
/// До начала работы e[-1] = e[-2] = 0, u[-1] = 0.
class PidController {
public:
    /// Задаёт коэффициенты Kp, Ki, Kd, шаг дискретизации dt и пределы
    /// исполнительного устройства [u_min, u_max].
    PidController(double kp, double ki, double kd, double dt,
                  double u_min = -10.0, double u_max = 10.0);

    /// Очередное управляющее воздействие по текущей ошибке e[t].
    double compute(double e);

    /// Сброс истории ошибок и накопленного управления.
    void reset();

    /// Последнее рассчитанное управляющее воздействие.
    double get_u() const;

    /// Коэффициент пропорциональной части Kp.
    double get_kp() const;
    /// Коэффициент интегральной части Ki.
    double get_ki() const;
    /// Коэффициент дифференциальной части Kd.
    double get_kd() const;

private:
    /// Ограничение u пределами исполнительного устройства.
    double clamp(double u) const;

    double kp;
    double ki;
    double kd;
    double dt;
    double u_min;
    double u_max;
    double e1 = 0.0;
    double e2 = 0.0;
    double u = 0.0;
};

#endif
