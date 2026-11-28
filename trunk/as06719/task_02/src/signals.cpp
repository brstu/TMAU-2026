#include "signals.h"

#include <cmath>
#include <stdexcept>
#include <vector>

std::vector<double> stepSignal(int N, double amplitude) {
    if (N < 0) {
        throw std::invalid_argument("N must be non-negative");
    }

    return std::vector<double>(N, amplitude);
}

std::vector<double> impulseSignal(int N, double amplitude) {
    if (N < 0) {
        throw std::invalid_argument("N must be non-negative");
    }

    std::vector signal(N, 0.0);

    if (!signal.empty()) {
        signal.front() = amplitude;
    }

    return signal;
}

std::vector<double> harmonicSignal(
    int N,
    double amplitude,
    double frequency
) {
    if (N < 0) {
        throw std::invalid_argument("N must be non-negative");
    }

    const double PI = std::acos(-1.0);
    std::vector<double> signal(N);

    for (int i = 0; i < N; ++i) {
        signal[i] = amplitude *
            std::sin(2.0 * PI * frequency * i);
    }

    return signal;
}
