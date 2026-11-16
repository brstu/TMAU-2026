#include "LinearModel13.h"
#include "NonlinearModel25.h"
#include "ODE37.h"
#include "Simulator.h"
#include "SignalGenerator.h"
#include <iostream>

int main() {
    std::cout << "Lab 2 App\n";
    
    LinearModel13 model(1.1, -0.5, 0.5);
    SignalGenerator signal(SignalType::STEP, 1.0);
    Simulator sim(std::make_unique<LinearModel13>(1.1, -0.5, 0.5), 5);
    
    auto res = sim.run(signal);
    for (double y : res) {
        std::cout << y << "\n";
    }
    return 0;
}