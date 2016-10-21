#include <avr/io.h>

int main(void)
{
    static float a = 43.75;
    int addr = &a;
    int i = 0;
    for (i = 0; i < 100; i++) {
        a = a + 0.01;
    }

    while(1){};
    return 0;
}
