#ifndef MEMORY_H
#define MEMORY_H

#include <iostream>
#include <string>
#include <array>
#include <cstdint>

using namespace std;

class Memory {
private:
    array<uint8_t, 256> cells;

public:
    Memory();
    uint8_t getCell(int address) const;
    void setCell(int address, uint8_t value);
    string getHexCell(int address) const;
    void setHexCell(int address, string hexByte);
    string getWord(int address) const;
    void clear();
    static bool isValidAddress(int address);
};

#endif
