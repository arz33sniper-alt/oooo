#include "Machine.h"

Machine::Machine() : cpu(&memory, &registers, &screen) {
}

bool Machine::loadProgram(string filePath, int startAddress) {
    if (!loader.loadFromFile(filePath, startAddress, memory))
        return false;
    cpu.reset(startAddress);
    return true;
}

bool Machine::step() {
    return cpu.step();
}

void Machine::runAll() {
    int steps = 0;
    while (!cpu.isHalted() && steps < 2000) {
        if (!cpu.step())
            break;
        steps++;
    }
}

void Machine::reset(int startAddress) {
    memory.clear();
    registers.clear();
    screen.clear();
    cpu.reset(startAddress);
}

bool Machine::overwriteMemoryCell(int address, string hexValue) {
    if (!Memory::isValidAddress(address) || !ProgramLoader::isValidHexByte(hexValue))
        return false;
    memory.setHexCell(address, hexValue);
    return true;
}

bool Machine::overwriteRegister(int regIndex, string hexValue) {
    if (!RegisterFile::isValidRegister(regIndex) || !ProgramLoader::isValidHexByte(hexValue))
        return false;
    registers.setHexRegister(regIndex, hexValue);
    return true;
}

const Memory& Machine::getMemory() const {
    return memory;
}

const RegisterFile& Machine::getRegisterFile() const {
    return registers;
}

const CPU& Machine::getCPU() const {
    return cpu;
}

const Screen& Machine::getScreen() const {
    return screen;
}
