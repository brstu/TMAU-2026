#include "input_signal.h"
#include <cmath>

using namespace std;

double InputSignal::getSignal(InputType type, int step, double amp) {
    if (type == InputType::Step) {
        return amp;
    }
    
    if (type == InputType::Impulse) {
        return (step == 1) ? amp : 0.0;
        
    }
    return amp * sin(static_cast<double>(step));
}