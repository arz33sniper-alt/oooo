#ifndef REGISTERFILE_H
#define REGISTERFILE_H

#include <iostream>
#include <string>
#include <array>
#include <cstdint>

using namespace std;

class RegisterFile {
private:
    array<uint8_t, 16> registers;

public:
    RegisterFile();
    uint8_t getRegister(int index) const;
    void setRegister(int index, uint8_t value);
    string getHexRegister(int index) const;
    void setHexRegister(int index, string hexByte);
    void clear();
    static bool isValidRegister(int index);
};

#endif
