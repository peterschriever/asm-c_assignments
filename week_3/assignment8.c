#include </home/peterzen/Code/AVR-C/avr8-gnu-toolchain/avr/include/avr/iom328p.h>
#include </home/peterzen/Code/AVR-C/avr8-gnu-toolchain/avr/include/avr/sfr_defs.h>

void init_ports(void);

int main(void) {
    init_ports();
    while(1) {
        if(PINB > 0) {
            PORTD = PINB;
        }
    }
}

void init_ports(void) {
    DDRB = 0x00; // set PortB input by default
    DDRD = 0xff; // set PortD, output by default
}
