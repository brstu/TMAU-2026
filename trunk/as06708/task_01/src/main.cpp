#include <iostream>
#include <iomanip>
#include <fstream>
#include <concepts> 
#include <functional> //std::function
#include <algorithm> //std::clamp

#include <memory> //std::unique_ptr
#include <vector>
#include <string_view>
#include <cmath>

#include "model_realizations.hpp"

template<typename T>
concept Number = std::is_integral_v<T> || std::is_floating_point_v<T>;


enum class SignalType {
    STEP = 1,
    IMPULSE = 2,
    HARMONIC = 3
};

enum class ModelType {
    LINEAR = 1,
    NONE_LINEAR = 2,
    SIMPLE_DIFF = 3
};

struct IterationData{
    size_t tau;
    double u;
    double y;
};

std::string_view signalTypeToString(SignalType type){
    switch (type)
    {
    case SignalType::STEP: return "Ступенчатый";
    case SignalType::IMPULSE: return "Импульсный";
    case SignalType::HARMONIC: return "Гармонический";
    default: return "";
    }
}

std::string_view modelTypeToString(ModelType type){
    switch (type)
    {
    case ModelType::LINEAR: return "Линейная модель";
    case ModelType::NONE_LINEAR: return "Нелинейная модель";
    case ModelType::SIMPLE_DIFF: return "Простая дифференциальная";
    default: return "";
    }
}

void printResult(std::vector<IterationData>& result, ModelType model, SignalType signal){
    std::cout << "===============================" <<std::endl;
    std::cout << "  Результаты работы программы  " <<std::endl;
    std::cout << "===============================" <<std::endl;
    std::cout << "Название модели: " << modelTypeToString(model) << std::endl;
    std::cout << "Тип сигнала: " << signalTypeToString(signal) << std::endl << std::endl;
    std::cout << std::setw(5) << "tau" << std::setw(3) << "u" << std::setw(10) << "y" << std::endl;
    for (const auto &i : result)
        std::cout << std::setw(5) << i.tau << std::setw(3) << i.u << std::setw(10) << std::setprecision(2) << i.y << std::endl;
}

double generateSignal(SignalType type, uint16_t tau){
    switch (type)
    {
        case SignalType::STEP: return 1.;
        case SignalType::IMPULSE: return (tau == 0u) ? 1. : 0.;
        case SignalType::HARMONIC: return std::sin(tau);
        default: return -1.0;
    }
}

template<Number T>
T readNumber(std::string_view prompt, std::function<bool(T)> checkCondition = nullptr){
    std::cout << prompt;
    T result = 0;

    while (true)
    {
        if (!(std::cin >> result)){
            std::cout << "Ошибка: неправильный формат числа!" << std::endl;
            std::cin.clear();
            std::cin.ignore(1000, '\n');
            continue;
        }

        //-------------------------------------
        // Валидация правильно введенного числа
        //-------------------------------------
        
        // Эта проверка создается только если в шаблоне указан тип с плавающей точкой
        if constexpr (std::is_floating_point_v<T>){
            if (!std::isfinite(result)){
                std::cout << "Ошибка: число является бесконечным!" << std::endl;
                continue;
            }
        }

        // Эта проверка создается только если в шаблоне указан безнаковый тип
        if constexpr (std::is_unsigned_v<T>){
            if (result == std::numeric_limits<T>::max()){
                std::cout << "Ошибка: отрицательное число в безнаковом типе!" << std::endl;
                continue;
            }
        }

        // Пользовательская проверка
        if (checkCondition == nullptr) return result;
        else if (checkCondition(result)) return result;
        
    }
    
}

static std::unique_ptr<IModel> createModel(ModelType type){
    switch (type)
    {
        case ModelType::LINEAR:
        {
            std::cout << "Создание линейной модели:" << std::endl;
            const double a1 = readNumber<double>("a1 = ");
            const double a2 = readNumber<double>("a2 = ");
            const double b1 = readNumber<double>("b1 = ");
            const double b2 = readNumber<double>("b2 = ");
            return std::make_unique<LinearModel>(a1, a2, b1, b2);
        }
        case ModelType::NONE_LINEAR:
        {
            std::cout << "Создание нелинейной модели:" << std::endl;
            const double a    = readNumber<double>("a = ");
            const double b    = readNumber<double>("b = ");
            const double uMin = readNumber<double>("uMin = ");
            const double uMax = readNumber<double>("uMax = ");
            return std::make_unique<NoneLinearModel>(a, b, uMin, uMax);
        }
        case ModelType::SIMPLE_DIFF:
        {
            std::cout << "Создание дифференциальной модели:" << std::endl;
            const double a  = readNumber<double>("a = ");
            const double b  = readNumber<double>("b = ");
            const double dt = readNumber<double>("dt = ");
            return std::make_unique<SimpleDifferentialModel>(a, b, dt);
        }
    default:
         return std::make_unique<LinearModel>(0.0, 0.0, 0.0, 0.0);
    }
}

bool saveToCsv(const std::vector<IterationData>& result, ModelType model, SignalType signal){
    std::ofstream resultFile("./result_data.csv");
    if (!resultFile.is_open()){
        std::cerr << "Не удалось создать файл!" << std::endl;
        return false;
    }

    resultFile << modelTypeToString(model) << ";" << signalTypeToString(signal) << std::endl;
    for (const auto& i : result)
        resultFile << i.tau << ";" << i.u << ";" << i.y << std::endl;
    
    resultFile.close();
    return true;
}

int main(){
    std::setlocale(LC_ALL, "ru_RU.UTF-8");
    std::cout << "===========================" <<std::endl;
    std::cout << "  Начало работы программы  " <<std::endl;
    std::cout << "===========================" <<std::endl;
    std::function<bool(int)> fromOneToThreeChoise = [](int value) -> bool {
                return value == std::clamp(value,
                    static_cast<int>(SignalType::STEP),
                    static_cast<int>(SignalType::HARMONIC)
                ); 
        };
    
    const auto signalChoise = static_cast<SignalType>(readNumber<int>(
            "Выберите тип сигнала: \n 1. Ступенчатый \n 2. Импульсный \n 3. Гармонический.\n>",
            fromOneToThreeChoise)
        );

    const auto modelChoise = static_cast<ModelType>(readNumber<int>(
            "Выберите тип модели: \n1. Линейная \n 2. Нелинейная \n 3. Простая дифференциальная.\n>",
            fromOneToThreeChoise)
        );
    
    std::cout << "========================" <<std::endl;
    std::cout << "  Этап создания модели " <<std::endl;
    std::cout << "========================" <<std::endl;
    std::unique_ptr<IModel> model = createModel(modelChoise);
    std::cout << "============================" <<std::endl;
    std::cout << "  Этап настройки симуляции  " <<std::endl;
    std::cout << "============================" <<std::endl;
    size_t iterationsNumber = readNumber<size_t>("Введите количество шагов симуляции: ");
    
    double y = 0.0;
    double yPrev = 0.0;
    y = readNumber<double>("Введите начальное значение y: ");
    yPrev = y;
    double u = 0.0;
    double uPrev = 0.0;
    
    std::vector<IterationData> iterations;
    // Цикл симуляции
    for (size_t tau = 0; tau < iterationsNumber; ++tau)
    {
        double nextU = generateSignal(signalChoise, tau);
        double nextY = model.get()->next_step(y, yPrev, u, uPrev);
        uPrev = u; u = nextU;
        yPrev = y; y = nextY;
        iterations.emplace_back(IterationData{tau, u, y});
    }
    printResult(iterations, modelChoise, signalChoise);
    if (saveToCsv(iterations, modelChoise, signalChoise))
        std::cout << "Данные сохранены в result_data.csv" << std::endl;
    
    std::system("python plot.py");
}