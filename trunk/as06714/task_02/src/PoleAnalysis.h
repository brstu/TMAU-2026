#pragma once

#include <iosfwd>
#include <stdexcept>
#include <string>
#include <vector>

// Анализ устойчивости моделей варианта 7 по расположению полюсов на Z-плоскости.
// Дискретная система сходится, когда спектральный радиус (наибольший модуль полюса)
// меньше единицы.
namespace stability {

    enum class Verdict {
        Converging,  // все полюса внутри единичного круга
        Marginal,    // наибольший полюс лежит на окружности единичного радиуса
        Diverging    // хотя бы один полюс снаружи круга
    };

    // Какая модель анализировалась; по нему печатается имя модели из строковых констант
    enum class Plant {
        Linear1_7,
        Sqrt2_9,
        Euler3_1
    };

    struct PoleReport {
        Plant plant = Plant::Linear1_7;
        std::string modelTag;          // имя модели для сообщений
        std::string polynomial;        // знаменатель передаточной функции
        std::vector<double> poles;     // у моделей варианта 7 все полюса вещественные
        double spectralRadius = 0.0;
        Verdict verdict = Verdict::Diverging;
    };

    // Ширина полосы вокруг единичной окружности, внутри которой полюс считается граничным
    inline constexpr double kUnitCircleEps = 1e-9;

    class DivergenceError : public std::runtime_error {
    public:
        using std::runtime_error::runtime_error;
    };

    // Наибольший модуль полюса; бесконечность, если среди полюсов есть NaN или inf
    double spectralRadius(const std::vector<double>& poles);

    // Вердикт по спектральному радиусу
    Verdict judge(double radius);

    // Model 1.7: двукратный полюс в нуле (задержки входа) и полюс, равный a
    PoleReport inspectModel1_7(double a);

    // Model 2.9: однородная часть линейна, единственный полюс a
    PoleReport inspectModel2_9(double a);

    // Model 3.1 после дискретизации Эйлером: единственный полюс, равный единице минус a на dt
    PoleReport inspectModel3_1(double a, double dt);

    bool converges(const PoleReport& report);

    std::string verdictText(Verdict verdict);

    // Печатает тревожное сообщение для несходящейся модели; true, если оно было напечатано
    bool printAlarm(const PoleReport& report, std::ostream& sink);

    // Имя модели — строковая константа, не зависящая от пользовательского ввода
    const char* plantTag(Plant plant);

    // Печатает знаменатель передаточной функции прямо из числовых полюсов.
    // В поток попадают только константы и числа, поэтому ввод пользователя
    // не может внести в вывод посторонний текст (защита от log injection).
    void writeDenominator(const PoleReport& report, std::ostream& sink);

    // Бросает DivergenceError, если модель не сходится
    void enforceConvergence(const PoleReport& report);

} // namespace stability
