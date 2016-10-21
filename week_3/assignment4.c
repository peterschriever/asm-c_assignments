#include </home/peterzen/Code/AVR-C/avr8-gnu-toolchain/avr/include/stdio.h>
int z;

void f(int x)
{
    x = 2;
    z+=x;
}

int main()
{
    z = 5;
    f(z);
    printf("z = %d\n", z);
}
