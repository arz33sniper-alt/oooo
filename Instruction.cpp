#include "Instruction.h"
#include <cctype>

static int hexToInt(char c) {
    c = toupper(c);
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return 10 + (c - 'A');
    return 0;
}

Instruction::Instruction(string hexStr) {
    if (hexStr.length() == 4) {
        rawHex = "";
        for (int i = 0; i < 4; i++) {
            rawHex += (char)toupper(hexStr[i]);
        }
        opcode = rawHex[0];
        d1 = hexToInt(rawHex[1]);
        d2 = hexToInt(rawHex[2]);
        d3 = hexToInt(rawHex[3]);
    } else {
        rawHex = "0000";
        opcode = '0';
        d1 = d2 = d3 = 0;
    }
}

bool Instruction::isValid(string hexStr) {
    if (hexStr.length() != 4)
        return false;

    for (int i = 0; i < 4; i++) {
        if (!isxdigit(hexStr[i]))
            return false;
    }

    char op = toupper(hexStr[0]);
    char c1 = toupper(hexStr[1]);
    char c2 = toupper(hexStr[2]);
    char c3 = toupper(hexStr[3]);

    if (!((op >= '1' && op <= '9') || op == 'A' || op == 'B' || op == 'C'))
        return false;

    if (op == '4' && c1 != '0')
        return false;

    if (op == 'A' && c2 != '0')
        return false;

    if (op == 'C' && (c1 != '0' || c2 != '0' || c3 != '0'))
        return false;

    return true;
}

string Instruction::getRawHex() const {
    return rawHex;
}

char Instruction::getOpcode() const {
    return opcode;
}

int Instruction::getR() const {
    return d1;
}

int Instruction::getS() const {
    return d2;
}

int Instruction::getT() const {
    return d3;
}

uint8_t Instruction::getXY() const {
    return (uint8_t)((d2 << 4) | d3);
}

int Instruction::getX() const {
    return d3;
}
