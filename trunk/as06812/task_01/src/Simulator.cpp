#include "Simulator.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;

Simulator::Simulator(int steps) : steps(steps) {}

void Simulator::run(Model& model, const string& modelName, const InputSignal& signal) const
{
    model.resetState();
    
    string inputName = signal.toString();
    string filename = "../doc/" + modelName + "_" + inputName + ".csv";
    
    ofstream out(filename);
    if (!out) 
    {
        cerr << "[Error] Cannot open " << filename << "\n";
        return;
    }

    out << "Step,u,y\n";
    cout << "\n>>> Data for " << modelName << " (" << inputName << " signal):\n";
    cout << setw(6) << "Step" << setw(12) << "u(t)" << setw(14) << "y(t)\n";

    for (int t = 0; t < steps; ++t)
    {
        double u = signal.getValue(t);
        double y = model.processStep(u);
        
        out << t << "," << u << "," << y << "\n";
        cout << setw(6) << t << setw(12) << fixed << setprecision(4) << u << setw(14) << y << "\n";
    }
}