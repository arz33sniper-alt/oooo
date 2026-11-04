#include "Memory.h"
#include <sstream>
#include <iomanip>
#include <cctype>

Memory::Memory() {
    clear();
}

uint8_t Memory::getCell(int address) const {
    if (!isValidAddress(address))
        return 0;
    return cells[address];
}

void Memory::setCell(int address, uint8_t value) {
    if (isValidAddress(address))
        cells[address] = value;
}

string Memory::getHexCell(int address) const {
    if (!isValidAddress(address))
        return "00";
    stringstream ss;
    ss << uppercase << hex << setw(2) << setfill('0') << (int)cells[address];
    return ss.str();
}

void Memory::setHexCell(int address, string hexByte) {
    if (!isValidAddress(address) || hexByte.length() != 2)
        return;
    if (!isxdigit(hexByte[0]) || !isxdigit(hexByte[1]))
        return;
    int val = stoi(hexByte, 0, 16);
    cells[address] = (uint8_t)val;
}

string Memory::getWord(int address) const {
    if (address < 0 || address >= 255)
        return "0000";
    return getHexCell(address) + getHexCell(address + 1);
}

void Memory::clear() {
    for (int i = 0; i < 256; i++) {
        cells[i] = 0;
    }
}

bool Memory::isValidAddress(int address) {
    return (address >= 0 && address < 256);
}
