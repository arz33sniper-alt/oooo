#include "ProgramLoader.h"
#include "Instruction.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <cctype>

bool ProgramLoader::loadFromFile(string filePath, int startAddress, Memory& memory) const {
    if (!isValidStartAddress(startAddress)) {
        cout << "Invalid start address! Must be an even address less than FF." << endl;
        return false;
    }

    ifstream file(filePath);
    if (!file.is_open()) {
        cout << "Error: Could not open file " << filePath << endl;
        return false;
    }

    vector<string> list;
    string rawLine;
    int lineNum = 0;

    while (getline(file, rawLine)) {
        lineNum++;
        string cleaned = cleanLine(rawLine);
        if (cleaned == "")
            continue;

        if (!Instruction::isValid(cleaned)) {
            cout << "Invalid instruction (" << cleaned << ") at line " << lineNum << ". Program rejected." << endl;
            return false;
        }
        list.push_back(cleaned);
    }

    if (list.empty()) {
        cout << "Error: File has no instructions." << endl;
        return false;
    }

    if (startAddress + (int)list.size() * 2 > 256) {
        cout << "Error: Program is too large for memory." << endl;
        return false;
    }

    int addr = startAddress;
    for (int i = 0; i < (int)list.size(); i++) {
        memory.setHexCell(addr, list[i].substr(0, 2));
        memory.setHexCell(addr + 1, list[i].substr(2, 2));
        addr += 2;
    }

    return true;
}

string ProgramLoader::cleanLine(string rawLine) {
    string line = rawLine;
    size_t pos = line.find(';');
    if (pos != string::npos) {
        line = line.substr(0, pos);
    }

    int start = 0;
    while (start < (int)line.length() && isspace(line[start])) {
        start++;
    }
    if (start == (int)line.length())
        return "";

    int end = start;
    while (end < (int)line.length() && !isspace(line[end])) {
        end++;
    }

    string token = line.substr(start, end - start);
    for (int i = 0; i < (int)token.length(); i++) {
        token[i] = toupper(token[i]);
    }
    return token;
}

bool ProgramLoader::isValidStartAddress(int address) {
    return (address >= 0 && address < 0xFF && address % 2 == 0);
}

bool ProgramLoader::isValidHexByte(string hexStr) {
    if (hexStr.length() != 2)
        return false;
    return (isxdigit(hexStr[0]) && isxdigit(hexStr[1]));
}
