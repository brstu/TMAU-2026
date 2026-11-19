#include <iostream>
#include <windows.h>

#include "app.hpp"

#ifndef DOC_DIR
#define DOC_DIR "doc"
#endif


int main() {
    SetConsoleOutputCP(CP_UTF8);

    return runApp(std::cin, std::cout, DOC_DIR);
}
