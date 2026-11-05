#ifndef PROGRAMLOADER_H
#define PROGRAMLOADER_H

#include <string>
#include "Memory.h"

using namespace std;

class ProgramLoader {
public:
    bool loadFromFile(string filePath, int startAddress, Memory& memory) const;
    static string cleanLine(string rawLine);
    static bool isValidStartAddress(int address);
    static bool isValidHexByte(string hexStr);
};

#endif
