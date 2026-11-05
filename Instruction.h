#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <iostream>
#include <string>
#include <cstdint>

using namespace std;

class Instruction {
private:
    string rawHex;
    char opcode;
    int d1;
    int d2;
    int d3;

public:
    Instruction(string hexStr = "0000");
    static bool isValid(string hexStr);
    string getRawHex() const;
    char getOpcode() const;
    int getR() const;
    int getS() const;
    int getT() const;
    uint8_t getXY() const;
    int getX() const;
};

#endif
