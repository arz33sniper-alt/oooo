#ifndef CONSOLEUI_H
#define CONSOLEUI_H

#include "Machine.h"

class ConsoleUI {
private:
    Machine& machine;

public:
    ConsoleUI(Machine& m);
    void start();
    void displayMenu() const;
    void displayFullState() const;
    void displayMemoryMatrix() const;
    void displayRegistersAndCPU() const;
    void displayScreen() const;

    void handleLoadProgram();
    void handleOverwriteMemory();
    void handleOverwriteRegister();
    void handleRunAtOnce();
    void handleRunStepByStep();
};

#endif
