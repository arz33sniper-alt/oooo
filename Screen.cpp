#include "Screen.h"
#include <sstream>
#include <iomanip>

Screen::Screen() {
    clear();
}

void Screen::write(uint8_t value) {
    stringstream ss;
    ss << uppercase << hex << setw(2) << setfill('0') << (int)value;
    hexLog.push_back(ss.str());

    if (value >= 32 && value <= 126) {
        asciiBuffer += (char)value;
    } else {
        asciiBuffer += '.';
    }
}

string Screen::getAsciiOutput() const {
    return asciiBuffer;
}

string Screen::getHexOutput() const {
    string out = "";
    for (int i = 0; i < (int)hexLog.size(); i++) {
        if (i > 0) out += " ";
        out += hexLog[i];
    }
    return out;
}

void Screen::clear() {
    hexLog.clear();
    asciiBuffer = "";
}
