#include <iostream>
#include <string>
#include <limits>
#include "InputSignal.hpp"
#include "Model_1_2.hpp"
#include "Model_2_4.hpp"
#include "Model_3_6.hpp"
#include "Simulator.hpp"

using namespace std;

void clearInputStream(const string& errorMsg) 
{
    cout << errorMsg << "\n";
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

int readInt(const string& prompt, int minValue, int maxValue) 
{
    int value;
    while (true) 
    {
        cout << prompt;
        if (cin >> value && value >= minValue && value <= maxValue) 
        {
            return value;
        }
        clearInputStream("[Error] Invalid input! Enter a valid integer among ["
                         + to_string(minValue) + "; " + to_string(maxValue) + "]");
    }
}

double readDouble(const string& prompt) 
{
    double value;
    while (true) 
    {
        cout << prompt;
        if (cin >> value) 
        {
            return value;
        }
        clearInputStream("[Error] Invalid input! Enter a valid double");
    }
}

int main() 
{
    cout << "=== Interactive Model Configuration (Variant 2) ===\n";
    
    int steps = readInt("Enter number of steps (n) [1-100]: ", 1, 100);
    
    cout << "\nSelect model to calculate:\n";
    cout << "1. Model 1.2 (Delay)\n";
    cout << "2. Model 2.4 (Logarithmic)\n";
    cout << "3. Model 3.6 (Cubic)\n";
    int choice = readInt("Enter choice (1-3): ", 1, 3);
    
    double a = readDouble("Enter coefficient a: ");
    double b = readDouble("Enter coefficient b: ");
    
    int k_delay = 0;
    if (choice == 1) {
        k_delay = readInt("Enter delay k [0-10]: ", 0, 10);
    }
    
    cout << "\nSelect input signal:\n";
    cout << "1. Step\n";
    cout << "2. Impulse\n";
    cout << "3. Harmonic\n";
    int sigChoice = readInt("Enter choice (1-3): ", 1, 3);
    
    InputSignal::Type sigType = InputSignal::Type::STEP;
    if (sigChoice == 2) sigType = InputSignal::Type::IMPULSE;
    else if (sigChoice == 3) sigType = InputSignal::Type::HARMONIC;
    InputSignal signal(sigType);
    
    Simulator simulator(steps);
    
    switch (choice) 
    {
        case 1: {
            Model_1_2 model(a, b, k_delay);
            simulator.run(model, "Model_1_2", signal);
            break;
        }
        case 2: {
            Model_2_4 model(a, b);
            simulator.run(model, "Model_2_4", signal);
            break;
        }
        case 3: {
            Model_3_6 model(a, b, 0.1);
            simulator.run(model, "Model_3_6", signal);
            break;
        }
        default:
            break;
    }

    return 0;
}