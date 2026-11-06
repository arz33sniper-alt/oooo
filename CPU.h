#ifndef CPU_H
#define CPU_H

#include <string>
#include "ALU.h"
#include "Instruction.h"
#include "Memory.h"
#include "RegisterFile.h"
#include "Screen.h"

using namespace std;

class CPU {
private:
    int pc;
    string ir;
    bool halted;
    ALU alu;
    Memory* memory;
    RegisterFile* registers;
    Screen* screen;

public:
    CPU(Memory* mem, RegisterFile* regs, Screen* scr);
    bool fetch();
    Instruction decode() const;
    void execute(const Instruction& instr);
    bool step();
    void reset(int startAddress = 0);
    int getPC() const;
    void setPC(int address);
    string getIR() const;
    bool isHalted() const;
};

#endif
