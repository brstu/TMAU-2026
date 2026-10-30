#include "ConsoleUI.h"
#include <windows.h>

int main()
{
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    ConsoleUI ui;
    ui.run();
    return 0;
}