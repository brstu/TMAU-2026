#include "ConsoleUI.h"
#include "simulation.h"
#include "LinearModel.h"
#include "SquareRootModel.h"
#include "PureLinearDecayModel.h"
#include "results_csv.h"

#include <iostream>
#include <limits>
#include <cstdlib>

int ConsoleUI::readChoice(int min, int max)
{
    int x;
    while (true)
    {
        std::cout << "> ";
        if (std::cin >> x && x >= min && x <= max)
            return x;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Enter a number from " << min << " to " << max << "\n";
    }
}

double ConsoleUI::readDouble(const std::string& prompt)
{
    double x;
    while (true)
    {
        std::cout << prompt;
        if (std::cin >> x)
            return x;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Please enter a valid number\n";
    }
}

int ConsoleUI::readSteps()
{
    std::cout << "Number of steps (1..1000):\n";
    return readChoice(1, 1000);
}

InputType ConsoleUI::selectImpact()
{
    std::cout << "Select input type:\n"
              << "  1 - Step\n"
              << "  2 - Impulse\n"
              << "  3 - Harmonic\n";

    switch (readChoice(1, 3))
    {
    case 1:  return InputType::Step;
    case 2:  return InputType::Impulse;
    default: return InputType::Harmonic;
    }
}

std::unique_ptr<Model> ConsoleUI::selectModel(std::string& name)
{
    std::cout << "Select a model:\n"
              << "  1 - Linear: y(j) = a*y(j-1) + b1*u(j) + b2*u(j-1) + b3*u(j-2)\n"
              << "  2 - Square root: y(j) = a*y(j-1) + b*sign(u)*sqrt(|u|)\n"
              << "  3 - Linear decay: y(j+1) = y(j) - h*a*y(j)\n";

    switch (readChoice(1, 3))
    {
    case 1:
    {
        name = "LinearModel";
        double a  = readDouble("a  = ");
        double b1 = readDouble("b1 = ");
        double b2 = readDouble("b2 = ");
        double b3 = readDouble("b3 = ");
        return std::make_unique<LinearModel>(a, b1, b2, b3);
    }
    case 2:
    {
        name = "SquareRootModel";
        double a = readDouble("a = ");
        double b = readDouble("b = ");
        return std::make_unique<SquareRootModel>(a, b);
    }
    default:
    {
        name = "PureLinearDecayModel";
        double a  = readDouble("a  = ");
        double h  = readDouble("h  = ");
        double y0 = readDouble("y0 = ");
        return std::make_unique<PureLinearDecayModel>(a, h, y0);
    }
    }
}

void ConsoleUI::run()
{
    int steps = readSteps();

    std::string modelName;
    auto model = selectModel(modelName);
    InputType type = selectImpact();

    Simulation sim(steps, type, modelName, std::move(model));
    auto result = sim.run();

    saveResultToCsv(result, "results.csv");
    std::cout << "Results saved to results.csv\n";

    if (std::system("python plot.py results.csv") != 0)
        std::cout << "Failed to run plot.py\n";
}
