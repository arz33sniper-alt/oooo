#include <iostream>
#include "Machine.h"
#include "ConsoleUI.h"

using namespace std;

int main() {
    Machine machine;
    ConsoleUI ui(machine);
    ui.start();
    return 0;
}
