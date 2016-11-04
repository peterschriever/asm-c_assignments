/*
 * distance.c
 *
 * Created: 29-6-2016 14:44:43
 *  Author: jacob
 */ 

/* 
 * HC-SR04
 * trigger : uno 0 (PD0) out
 * echo    : uno 3 (PD3) INT1 in
 * 
 * DIO : uno 8  (PB0) data
 * CLK : uno 9  (PB1) clock
 * STB : uno 10 (PB2) strobe
 *
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 16E6
#include <util/delay.h>
#include "distance.h"

volatile uint16_t gv_counter; // 16 bit
volatile uint8_t echo; // a flag

void init_ports(void)
{
    DDRB=0xff; // set port B as output
    PORTB=0x00;
    DDRD=0x01; // set port D0 as output, D3 as input
    PORTD = 0x00; // clear bit D0
	_delay_us(2);
}

void init_timer(void)
// prescale, no interrupt, counting up
{
    // prescaling : max time = 2^16/16E6 = 4.1 ms, 4.1 >> 2.3, so no prescaling required
    TCCR1A = 0;
    TCCR1B = _BV(CS10);
}

void init_ext_int(void)
{
    // any change triggers ext interrupt 1
    EICRA = (1 << ISC10);
    EIMSK = (1 << INT1);
}

void reset()
{
	// clear memory - all 16 addresses
	sendCommand(0x40); // set auto increment mode
	write(strobe, LOW);
	shiftOut(0xc0);   // set starting address to 0
	for(uint8_t i = 0; i < 16; i++)
	{
		shiftOut(0x00);
	}
	write(strobe, HIGH);
}

void show_distance(uint16_t cm)
{
    // TCNT1 is 16 bits, int is 15 bits, uint16_t is 16 bits
 
               /*0*/  /*1*/   /*2*/  /*3*/  /*4*/  /*5*/  /*6*/  /*7*/   /*8*/  /*9*/
    uint8_t digits[] = { 0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f };
	uint8_t display[7] = {0};
	
    uint8_t digit = 0, i = 0, max = 0;
	
	while (cm > 0) {
		digit = cm % 10;
		display[i] = digit;
		cm = cm / 10;
		i++;
	}
	
	max = i;

    sendCommand(0x40); // auto-increment address
    write(strobe, LOW);
    shiftOut(0xc0); // set starting address = 0
	// show in reverse order
	for (i=0; i<max; i++) {
	    shiftOut(digits[display[max-1-i]]);
	    shiftOut(0x00);
	}    

    write(strobe, HIGH);
}

void sendCommand(uint8_t value)
{
    write(strobe, LOW);
    shiftOut(value);
    write(strobe, HIGH);
}

// write value to pin
void write(uint8_t pin, uint8_t val)
{
    if (val == LOW) {
        PORTB &= ~(_BV(pin)); // clear bit
    } else {
        PORTB |= _BV(pin); // set bit
    }
}

// shift out value to data
void shiftOut (uint8_t val)
{
    uint8_t i;
    for (i = 0; i < 8; i++)  {
        write(clock, LOW);   // bit valid on rising edge
        write(data, val & 1 ? HIGH : LOW); // lsb first
        val = val >> 1;
        write(clock, HIGH);
    }
}

uint16_t calc_cm(uint16_t counter)
{
    // counter 0 ... 65535, f = 16 MHz
    uint16_t micro_sec = counter/16;
    // micro_sec 0..4095 cm 0..70
    return (micro_sec / 58.2);
}

int main(void)
{
    uint16_t cm = 0;
    
    init_ports();
	reset(); // init LEDs
    init_timer();
    init_ext_int();
	sei();
    sendCommand(0x89);  // activate and set brightness to medium
    _delay_ms(50);
	while(1) {
        echo = BEGIN; // set flag
        // start trigger puls lo -> hi
        PORTD |= _BV(0); // set bit D0
 		_delay_us(12); // micro sec
        // stop trigger puls hi -> lo
        PORTD &=~ _BV(0); // clear bit D0
        _delay_ms(20); // milli sec, timer1 is read in ISR
        cm = calc_cm(gv_counter);
        show_distance(cm);
        _delay_ms(500); // milli sec
    }
}

ISR (INT1_vect)
{
    if (echo == BEGIN) {
        // set timer1 value to zero
        TCNT1 = 0;
        // clear flag
        echo = END;
    } else {
        // read value timer1
        gv_counter = TCNT1;
    }
}