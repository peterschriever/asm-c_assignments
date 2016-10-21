#include </home/peterzen/Code/AVR-C/avr8-gnu-toolchain/avr/include/avr/iom328p.h>
#define F_CPU 16E6 // used in _delay_ms, 16 MHz
#include </home/peterzen/Code/AVR-C/avr8-gnu-toolchain/avr/include/util/delay.h>
#include </home/peterzen/Code/AVR-C/avr8-gnu-toolchain/avr/include/avr/sfr_defs.h>

#define OFF 0x00
#define ON 0x01

// connect 4 switches with port B0..B3 (total 16 pin-codes)
// connect 4 LEDs with port D0..D3
// set the pincode
// switches are numbered 0..3
const int8_t pincode[4] = {ON, OFF, ON, OFF}; // matching B0..B3

void init_ports(void);

int main(void)
{
    uint8_t input, i, match;
    init_ports();
    while(1) {
        // read B0..B3
        // check if code == pincode
        match = 0;
        input = PINB;
        // if(input & (_BV(PORTB0)|_BV(PORTB1)) ) {
        //     match = 1;
        // }

        for (i=0; i<3; i++) {
            // check if there is a match
            if((input & 1) == pincode[i]) {
                match = 1;
            } else {
                match = 0;
                break;
            }
            input = input >> 1;
        } // for
        if (match) {
            // all four switches match pincode, unlock door
            PORTD=0x0f; // LEDs on
            // delay 2 sec
            _delay_ms(2000);
            PORTD=0x00; // LEDs off
        } // if

        // delay 100 ms to avoid switch bouncing
        _delay_ms(100);
    } // while(1)
}

void init_ports(void)
{
    DDRB = 0x00; // set port B0..B3 as input
    DDRD = 0xff; // set port D as output
    PORTD = 0x00; // LEDs off
}
