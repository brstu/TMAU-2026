#include "Model_1_2.hpp"

using namespace std;

Model_1_2::Model_1_2(double a_val, double b_val, int k_val) 
    : a(a_val), b(b_val), k(k_val) {}

double Model_1_2::processStep(double u) 
{
    u_history.push(u);
    double u_delayed = 0.0;

    if (u_history.size() > static_cast<size_t>(k)) 
    {
        u_delayed = u_history.front();
        u_history.pop();
    }
    
    double y_next = a * getY() + b * u_delayed;
    setY(y_next);
    return y_next;
}

void Model_1_2::resetState() 
{
    setY(0.0);
    u_history = queue<double>();
}