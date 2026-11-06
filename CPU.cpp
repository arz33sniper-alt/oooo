#include "CPU.h"

CPU::CPU(Memory* mem, RegisterFile* regs, Screen* scr) {
    memory = mem;
    registers = regs;
    screen = scr;
    pc = 0;
    ir = "0000";
    halted = false;
}

bool CPU::fetch() {
    if (halted || pc < 0 || pc >= 255) {
        halted = true;
        return false;
    }
    ir = memory->getWord(pc);
    if (!Instruction::isValid(ir)) {
        halted = true;
        return false;
    }
    pc += 2;
    return true;
}

Instruction CPU::decode() const {
    return Instruction(ir);
}

void CPU::execute(const Instruction& instr) {
    char op = instr.getOpcode();
    int r = instr.getR();
    int s = instr.getS();
    int t = instr.getT();
    uint8_t xy = instr.getXY();

    if (op == '1') {
        registers->setRegister(r, memory->getCell(xy));
    }
    else if (op == '2') {
        registers->setRegister(r, xy);
    }
    else if (op == '3') {
        uint8_t val = registers->getRegister(r);
        memory->setCell(xy, val);
        if (xy == 0xFF || xy == 0x00) {
            screen->write(val);
        }
    }
    else if (op == '4') {
        uint8_t val = registers->getRegister(s);
        registers->setRegister(t, val);
    }
    else if (op == '5') {
        uint8_t res = alu.addTwosComplement(registers->getRegister(s), registers->getRegister(t));
        registers->setRegister(r, res);
    }
    else if (op == '6') {
        uint8_t res = alu.addFloatingPoint(registers->getRegister(s), registers->getRegister(t));
        registers->setRegister(r, res);
    }
    else if (op == '7') {
        uint8_t res = alu.bitwiseOr(registers->getRegister(s), registers->getRegister(t));
        registers->setRegister(r, res);
    }
    else if (op == '8') {
        uint8_t res = alu.bitwiseAnd(registers->getRegister(s), registers->getRegister(t));
        registers->setRegister(r, res);
    }
    else if (op == '9') {
        uint8_t res = alu.bitwiseXor(registers->getRegister(s), registers->getRegister(t));
        registers->setRegister(r, res);
    }
    else if (op == 'A') {
        uint8_t res = alu.rotateRight(registers->getRegister(r), instr.getX());
        registers->setRegister(r, res);
    }
    else if (op == 'B') {
        if (registers->getRegister(r) == registers->getRegister(0)) {
            pc = xy;
        }
    }
    else if (op == 'C') {
        halted = true;
    }
}

bool CPU::step() {
    if (!fetch())
        return false;
    Instruction instr = decode();
    execute(instr);
    return !halted;
}

void CPU::reset(int startAddress) {
    pc = startAddress;
    ir = "0000";
    halted = false;
}

int CPU::getPC() const {
    return pc;
}

void CPU::setPC(int address) {
    if (Memory::isValidAddress(address))
        pc = address;
}

string CPU::getIR() const {
    return ir;
}

bool CPU::isHalted() const {
    return halted;
}
