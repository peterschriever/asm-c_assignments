#include </home/peterzen/Code/AVR-C/avr8-gnu-toolchain/avr/include/avr/iom328p.h>

int max(int a, int b, int c);

int main(void) {
    int a = 30;
    int b = 2;
    int c = 13;
    int maxVal = max(a, b, c);
    while (1) {
    }
}

int max(int a, int b, int c) {
    int tempMax = 0;
    if(a >= b) {
        tempMax = a;
    } else {
        tempMax = b;
    }
    if(tempMax >= c) {
        return tempMax;
    }
    return c;
}
