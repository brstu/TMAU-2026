#pragma once
#include <ostream>

enum class Stability
{
    Stable,     // |a| < 1  - процесс затухает
    Boundary,   // |a| = 1  - процесс не затухает 
    Unstable    // |a| > 1  - процесс расходится
};

Stability checkStability(double a);

// true только при |a| < 1 
bool isStable(double a);


bool warnIfUnstable(double a, std::ostream& out);
