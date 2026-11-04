#ifndef SCREEN_H
#define SCREEN_H

#include <iostream>
#include <string>
#include <vector>
#include <cstdint>

using namespace std;

class Screen {
private:
    vector<string> hexLog;
    string asciiBuffer;

public:
    Screen();
    void write(uint8_t value);
    string getAsciiOutput() const;
    string getHexOutput() const;
    void clear();
};

#endif
