#include <iostream>

#include "app.hpp"

#ifdef _WIN32
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#ifndef DOC_DIR
#define DOC_DIR "doc"
#endif


int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    return runApp(std::cin, std::cout, DOC_DIR);
}
