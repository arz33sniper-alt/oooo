#ifndef MACHINE_H
#define MACHINE_H

#include <string>
#include "Memory.h"
#include "RegisterFile.h"
#include "Screen.h"
#include "CPU.h"
#include "ProgramLoader.h"

using namespace std;

class Machine {
private:
    Memory memory;
    RegisterFile registers;
    Screen screen;
    CPU cpu;
    ProgramLoader loader;

public:
    Machine();
    bool loadProgram(string filePath, int startAddress);
    bool step();
    void runAll();
    void reset(int startAddress = 0);
    bool overwriteMemoryCell(int address, string hexValue);
    bool overwriteRegister(int regIndex, string hexValue);

    const Memory& getMemory() const;
    const RegisterFile& getRegisterFile() const;
    const CPU& getCPU() const;
    const Screen& getScreen() const;
};

#endif
