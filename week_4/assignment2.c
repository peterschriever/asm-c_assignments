/**
 * reusing :
 * - bjab, march 2016 demo_TM1638.c
 * - AVR_TTC_scheduler.c
 * - AVR_TTC_scheduler.h
 * to keep things simple, we'll ony use PORTB
 *
 * Vcc : +5V, GND : ground
 * DIO : data (board pin 8)     (PB0)
 * CLK : clock (board pin 9)    (PB1)
 * STB : strobe (board pin 10)  (PB2)
 */

#include <avr/io.h>
#define F_CPU 16E6
#include <util/delay.h>
#include "AVR_TTC_scheduler.h"

#define HIGH 0x1
#define LOW  0x0

const uint8_t data = 0;
const uint8_t clock = 1;
const uint8_t strobe = 2;
uint8_t counterIndex;
uint8_t counting = 0;

void write(uint8_t pin, uint8_t val);
int read(uint8_t pin);
void shiftOut (uint8_t val);
uint8_t shiftIn (void);
void sendCommand(uint8_t value);
void resetLeds();

int read(uint8_t pin)
{
    if (PINB & _BV(pin)) { // if pin set in port
        return HIGH;
    } else {
        return LOW;
    }
}

void write(uint8_t pin, uint8_t val) {
    if (val == LOW) {
        PORTB &= ~(_BV(pin)); // clear bit
    } else {
        PORTB |= _BV(pin); // set bit
    }
}

void shiftOut (uint8_t val) {
    uint8_t i;
    for (i = 0; i < 8; i++)  {
        write(clock, LOW);   // bit valid on rising edge
        write(data, val & 1 ? HIGH : LOW); // lsb first
        val = val >> 1;
        write(clock, HIGH);
    }
}

void sendCommand(uint8_t value) {
    write(strobe, LOW);
    shiftOut(value);
    write(strobe, HIGH);
}

void countOneStep() {
    /*0*/  /*1*/   /*2*/  /*3*/  /*4*/  /*5*/  /*6*/  /*7*/   /*8*/  /*9*/
    uint8_t digits[] = { 0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f };

    static uint8_t digit = 0;

    sendCommand(0x40); // auto-increment address
    write(strobe, LOW);
    shiftOut(0xc0); // set starting address = 0
    for(uint8_t position = 0; position < 8; position++) {
        shiftOut(digits[digit]);
        shiftOut(0x00); // write 0 byte to end transmission
    }

    write(strobe, HIGH);

    digit = (digit + 1) % 10;
    // return (digit == 0);
}

uint8_t shiftIn(void)
{
    uint8_t value = 0;
    uint8_t i;

    DDRB &= ~(_BV(data)); // clear bit, direction = input

    for (i = 0; i < 8; ++i) {
        write(clock, LOW);   // bit valid on rising edge
        value = value | (read(data) << i); // lsb first
        write(clock, HIGH);
    }

    DDRB |= _BV(data); // set bit, direction = output

    return value;
}

// write value to LED
void setLed(uint8_t value, uint8_t position)
{
    sendCommand(0x44); // write position to fixed address 1
    write(strobe, LOW);
    shiftOut(0xC1 + (position << 1));
    shiftOut(value);
    write(strobe, HIGH);
}

uint8_t testButtons() {
    uint8_t buttons = 0;
    write(strobe, LOW);
    shiftOut(0x42); // key scan (read buttons)

    DDRB &= ~(_BV(data)); // clear bit, direction = input

    for (uint8_t i = 0; i < 4; i++) {
        uint8_t v = shiftIn() << i;
        buttons |= v;
    }

    DDRB |= _BV(data); // set bit, direction = output
    write(strobe, HIGH);
    return buttons;
}

// calls an action for the corresponding button, if pressed
void callActionForBtn() {
    uint8_t buttons = testButtons();

    uint8_t swA, swB;
    swA = _BV(0);
    swB = _BV(7);

    if (buttons & swA) {
      // startCounter
      // counterIndex = SCH_Add_Task(countOneStep, 0, 50); // countOneStep every 50ms
      counting = 1;
      setLed(1, 0);
      return;
    }
    if (buttons & swB) {
      // stopCounter
      // SCH_Delete_Task(counterIndex); // stop counting
      counting = 0;
      setLed(1, 7);
      return;
    }
    resetLeds();
}

void resetLeds() {
  setLed(0, 0);
  setLed(0, 7);
}

void setup() {
    DDRB=0xff; // set port B as output
    sendCommand(0x89);  // activate and set brightness to medium
    SCH_Init_T1();
    SCH_Start();
    SCH_Add_Task(callActionForBtn, 0, 10);
    // counterIndex = SCH_Add_Task(countOneStep, 0, 15);
    // SCH_Delete_Task(counterIndex);
}

int main(void) {
    setup();
    countOneStep();
    while (1) {
        if (counting) {
          _delay_ms(50);
          countOneStep();
        }
        SCH_Dispatch_Tasks();
    }
    return(0);
}
