#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <cmath>
#include "Models.h"

int main() {
    int n = 30;
    double dt = 0.1;
    
    Model1_7 m1(0.8, 0.5, 0.2, 0.1);
    Model2_1 m2(0.7, 0.1, 1.0, 0.5);
    Model3_5 m3(1.5, dt);

    if (std::abs(0.8) >= 1.0) std::cout << "WARNING: Model 1.7 UNSTABLE!\n";

    std::vector<Model*> models = {&m1, &m2, &m3};
    std::vector<std::string> names = {"Model 1.7", "Model 2.1", "Model 3.5"};
    std::vector<std::string> input_types = {"Step", "Impulse", "Harmonic"};

    for (const auto& input_type : input_types) {
        std::string filename = "output_" + input_type + ".csv";
        std::ofstream file(filename);
        
        file << "Step,u,";
        for (const auto& name : names) {
            file << name << ",";
        }
        file << "\n";

        for (auto* m : models) {
            m->reset();
        }

        std::cout << "--- Input: " << input_type << " ---\n";
        std::cout << "t\tu\t";
        for (const auto& name : names) {
            std::cout << name << "\t";
        }
        std::cout << "\n";

        for (int t = 0; t < n; ++t) {
            double u = 0.0;
            if (input_type == "Step") {
                u = 1.0; 
            } else if (input_type == "Impulse") {
                u = (t == 0) ? 1.0 : 0.0;
            } else if (input_type == "Harmonic") {
                u = std::sin(t * dt);
            }

            file << t << "," << u << ",";
            std::cout << t << "\t" << u << "\t";

            for (auto* m : models) {
                double y = m->step(u);
                file << y << ",";
                std::cout << y << "\t";
            }
            file << "\n";
            std::cout << "\n";
        }
        file.close();
    }
    return 0;
}