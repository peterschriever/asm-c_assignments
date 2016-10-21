#include </home/peterzen/Code/AVR-C/avr8-gnu-toolchain/avr/include/avr/iom328p.h>
#include </home/peterzen/Code/AVR-C/avr8-gnu-toolchain/avr/include/stdint.h>

uint8_t set_bit(uint8_t byte, uint8_t bitToSet);
uint8_t clear_bit(uint8_t byte, uint8_t bitToClear);
uint8_t invert(uint8_t byte);

struct DoubleInt8 {
    uint8_t a;
    uint8_t b;
};

struct DoubleInt8 count(uint8_t n);

int main(void) {
    uint8_t aByte = 0b11111110;
    aByte = set_bit(aByte, 0); // set the first bit in variable aByte
    aByte = clear_bit(aByte, 2); // clear the third bit in variable aByte
    while (1) {
    }
}

uint8_t set_bit(uint8_t byte, uint8_t bitToSet) {
    return byte | (1 << bitToSet);
}

uint8_t clear_bit(uint8_t byte, uint8_t bitToClear) {
    return byte & ~(1 << bitToClear);
}

uint8_t invert(uint8_t byte) {
    return ~byte;
}

struct DoubleInt8 count(uint8_t n) {
    struct DoubleInt8 result;
    // 'parallel' or 'variable-precision SWAR algorithm'
    // Algorithm source: http://stackoverflow.com/questions/109023/how-to-count-the-number-of-set-bits-in-a-32-bit-integer?page=1&tab=votes#tab-top
    n = n - ((n >> 1) & 0x55555555);
    n = (n & 0x33333333) + ((n >> 2) & 0x33333333);
    result.a = (((n + (n >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
    result.b = result.a - 8; // assume we are dealing uint8_t (8 bit integer)
    return result; // result.a contains the ones count, result.b the zeros
}
