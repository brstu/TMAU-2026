#ifndef TASK_02_SIGNALS_H
#define TASK_02_SIGNALS_H

#include <vector>

std::vector<double> stepSignal(int N, double amplitude = 1.0);
std::vector<double> impulseSignal(int N, double amplitude = 1.0);
std::vector<double> harmonicSignal(
    int N,
    double amplitude = 1.0,
    double frequency = 1.0
);

#endif // TASK_02_SIGNALS_H