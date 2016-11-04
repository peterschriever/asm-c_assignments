// "echo" program
#include <avr/io.h>
#include <stdlib.h>
#include <avr/sfr_defs.h>
#define F_CPU 16E6
#include <util/delay.h>

// output on USB = PD1 = board pin 1
// datasheet p.190; F_OSC = 16 MHz & baud rate = 19.200
#define UBBRVAL 51

void uart_init() {
  // set the baud rate
  UBRR0H = 0;
  UBRR0L = UBBRVAL;
  // disable U2X mode
  UCSR0A = 0;
  // enable transmitter and receiver
  UCSR0B = _BV(TXEN0)|_BV(RXEN0);
  // set frame format : asynchronous, 8 data bits, 1 stop bit, no parity
  UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);
}

char uart_getByte(void) {
    loop_until_bit_is_set(UCSR0A, RXC0); /* Wait until data exists. */
    return UDR0;
}

void uart_putByte(uint8_t c) {
    loop_until_bit_is_set(UCSR0A, UDRE0); /* Wait until data register empty. */
    UDR0 = c;
}

void setLeds(uint8_t ledsByte) {
  PORTB = ledsByte;
}

void init_ports() {
  DDRB = (_BV(PORTB0)|_BV(PORTB1)|_BV(PORTB2)); // set PortB pins to output
  PORTB = 0xff; // leds off
}

int main() {
  uart_init();
  init_ports();
  _delay_ms(1000);
  uint8_t getByte;
  while(1) {
    // read a char
    getByte = uart_getByte();
    setLeds(~getByte);
  }
  return 0;
}
