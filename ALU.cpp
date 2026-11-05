#include "ALU.h"
#include <cmath>

uint8_t ALU::addTwosComplement(uint8_t a, uint8_t b) const {
    return (uint8_t)(a + b);
}

uint8_t ALU::addFloatingPoint(uint8_t a, uint8_t b) const {
    double valA = decodeFloat(a);
    double valB = decodeFloat(b);
    return encodeFloat(valA + valB);
}

uint8_t ALU::bitwiseOr(uint8_t a, uint8_t b) const {
    return (uint8_t)(a | b);
}

uint8_t ALU::bitwiseAnd(uint8_t a, uint8_t b) const {
    return (uint8_t)(a & b);
}

uint8_t ALU::bitwiseXor(uint8_t a, uint8_t b) const {
    return (uint8_t)(a ^ b);
}

uint8_t ALU::rotateRight(uint8_t value, int steps) const {
    int s = steps % 8;
    if (s < 0) s += 8;
    if (s == 0) return value;
    return (uint8_t)((value >> s) | (value << (8 - s)));
}

double ALU::decodeFloat(uint8_t byte) {
    if ((byte & 0x7F) == 0)
        return 0.0;

    int sign = (byte >> 7) & 1;
    int exp = ((byte >> 4) & 7) - 4;
    int mant = byte & 15;

    double res = (mant / 16.0) * pow(2.0, exp);
    if (sign == 1)
        res = -res;
    return res;
}

uint8_t ALU::encodeFloat(double value) {
    if (abs(value) < 1e-9)
        return 0;

    int sign = 0;
    if (value < 0) {
        sign = 1;
        value = -value;
    }

    int exp = 0;
    double frac = value;

    while (frac >= 1.0 && exp < 3) {
        frac /= 2.0;
        exp++;
    }
    while (frac < 0.5 && frac > 0.0 && exp > -4) {
        frac *= 2.0;
        exp--;
    }

    int mant = (int)round(frac * 16.0);
    if (mant >= 16) {
        if (exp < 3) {
            mant /= 2;
            exp++;
        } else {
            mant = 15;
        }
    }

    int biasedExp = exp + 4;
    if (biasedExp < 0) biasedExp = 0;
    if (biasedExp > 7) biasedExp = 7;

    return (uint8_t)((sign << 7) | ((biasedExp & 7) << 4) | (mant & 15));
}
