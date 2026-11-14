#include <iostream>
#include <fstream>
#include <memory>
#include <vector>
#include <string>
#include "models.hpp"

void run_and_export(Model& model, const std::string& model_name, int steps, const std::string& filename) {
    std::ofstream csv(filename);
    csv << "tau,u_step,y_step,u_impulse,y_impulse,u_harmonic,y_harmonic\n";

    auto res_step = model.simulate(steps, InputType::Step);
    auto res_imp  = model.simulate(steps, InputType::Impulse);
    auto res_harm = model.simulate(steps, InputType::Harmonic);

    std::cout << "Результаты симуляции: " << model_name << std::endl;
    std::cout << "==========================================" << std::endl;
    std::cout << "tau\t| u(Step)\ty(Step)\t| u(Imp)\ty(Imp)\t| u(Harm)\ty(Harm)" << std::endl;
    std::cout << "----------------------------------------------------------------------" << std::endl;

    for (int i = 0; i < steps; ++i) {
        // Консольный вывод первых 10 шагов для наглядности
        if (i < 10 || i == steps - 1) {
            std::cout << i << "\t| "
                      << res_step[i].u << "\t" << res_step[i].y << "\t| "
                      << res_imp[i].u  << "\t" << res_imp[i].y  << "\t| "
                      << res_harm[i].u << "\t" << res_harm[i].y << std::endl;
        } else if (i == 10) {
            std::cout << "...\t| ...\t\t...\t| ...\t\t...\t| ...\t\t..." << std::endl;
        }

        // Запись в CSV
        csv << i << ","
            << res_step[i].u << "," << res_step[i].y << ","
            << res_imp[i].u  << "," << res_imp[i].y  << ","
            << res_harm[i].u << "," << res_harm[i].y << "\n";
    }

    std::cout << "ланные сохранены в файл: " << filename << std::endl;
}

int main() {
    int steps = 30;
    std::cout << "введите количество шагов симуляции N (по умолчанию 30): ";
    if (!(std::cin >> steps) || steps <= 0) {
        steps = 30;
    }

    Model1_8 m1;
    Model2_2 m2;
    Model3_6 m3;

    run_and_export(m1, "Model 1.8 (Autoregressive Linear)", steps, "model1_8_results.csv");
    run_and_export(m2, "Model 2.2 (Actuator Saturation)", steps, "model2_2_results.csv");
    run_and_export(m3, "Model 3.6 (Cubic Growth)", steps, "model3_6_results.csv");

    return 0;
}