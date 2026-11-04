#include "RegisterFile.h"
#include <sstream>
#include <iomanip>
#include <cctype>

RegisterFile::RegisterFile() {
    clear();
}

uint8_t RegisterFile::getRegister(int index) const {
    if (!isValidRegister(index))
        return 0;
    return registers[index];
}

void RegisterFile::setRegister(int index, uint8_t value) {
    if (isValidRegister(index))
        registers[index] = value;
}

string RegisterFile::getHexRegister(int index) const {
    if (!isValidRegister(index))
        return "00";
    stringstream ss;
    ss << uppercase << hex << setw(2) << setfill('0') << (int)registers[index];
    return ss.str();
}

void RegisterFile::setHexRegister(int index, string hexByte) {
    if (!isValidRegister(index) || hexByte.length() != 2)
        return;
    if (!isxdigit(hexByte[0]) || !isxdigit(hexByte[1]))
        return;
    int val = stoi(hexByte, 0, 16);
    registers[index] = (uint8_t)val;
}

void RegisterFile::clear() {
    for (int i = 0; i < 16; i++) {
        registers[i] = 0;
    }
}

bool RegisterFile::isValidRegister(int index) {
    return (index >= 0 && index < 16);
}
