/*
 * tm1638.c
 * bjab, march 2016
 * demo program interfacing TM1638 from Arduino UNO (ATmga328P)
 * reusing :
 *   -http://blog.3d-logic.com/2015/01/10/using-a-tm1638-based-board-with-arduino/
 *   -https://android.googlesource.com/platform/external/arduino/+/jb-mr1-dev/hardware/arduino/cores/arduino
 *
 * to keep things simple, we'll ony use PORTB
 *
 * Vcc : +5V, GND : ground
 * DIO : data (board pin 8)     (PB0)
 * CLK : clock (board pin 9)    (PB1)
 * STB : strobe (board pin 10) (PB2)
 *
 */

#include <avr/io.h>
#include <stdint.h>
#define F_CPU 16E6
#include <util/delay.h>

#define HIGH 0x1
#define LOW  0x0

const uint8_t data = 0;
const uint8_t clock = 1;
const uint8_t strobe = 2;

uint8_t counting(void); // need to put them in .h
uint8_t scroll(void);
uint8_t readButtons(void);

// read value from pin
int read(uint8_t pin)
{
    if (PINB & _BV(pin)) { // if pin set in port
        return HIGH;
    } else {
        return LOW;
    }
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

// shift in value from data
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

void sendCommand(uint8_t value)
{
    write(strobe, LOW);
    shiftOut(value);
    write(strobe, HIGH);
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

void setup()
{
     DDRB=0xff; // set port B as output

    sendCommand(0x89);  // activate and set brightness to medium
//  reset();
}

uint8_t counting()
{
                                 /*0*/  /*1*/   /*2*/  /*3*/  /*4*/  /*5*/  /*6*/  /*7*/   /*8*/  /*9*/
    uint8_t digits[] = { 0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f };

    static uint8_t digit = 0;

    sendCommand(0x40); // auto-increment address
    write(strobe, LOW);
    shiftOut(0xc0); // set starting address = 0
    for(uint8_t position = 0; position < 8; position++)
    {
        shiftOut(digits[digit]);
        _delay_ms(100);
        shiftOut(0x00);
    }

    write(strobe, HIGH);

    digit = (digit + 1) % 10;
    return (digit == 0);
}

uint8_t scroll()
{
     uint8_t scrollText[] =
    {
        /* */ /* */ /* */ /* */ /* */ /* */ /* */ /* */
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        /*H*/ /*E*/ /*L*/ /*L*/ /*O*/ /*.*/ /*.*/ /*.*/
        0x76, 0x79, 0x38, 0x38, 0x3f, 0x80, 0x80, 0x80,
        /* */ /* */ /* */ /* */ /* */ /* */ /* */ /* */
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        /*b*/ /*O*/ /*S*/ /*S*/ /*.*/ /*.*/ /*.*/ /*.*/
        0x7c, 0x3f, 0x6d, 0x6d, 0x80, 0x80, 0x80, 0x80,
    };

    static uint8_t index = 0;
    uint8_t scrollLength = sizeof(scrollText);

    sendCommand(0x40);
    write(strobe, LOW);
    shiftOut(0xc0);

    for(int i = 0; i < 8; i++)
    {
        uint8_t c = scrollText[(index + i) % scrollLength];
        shiftOut(c);
        shiftOut(c != 0 ? 1 : 0);
    }

    write(strobe, HIGH);

    index = (index + 1) % (scrollLength << 1);

    return (index == 0);
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

void buttons()
{
    // uint8_t promptText[] =
    // {
    //     /*P*/ /*r*/ /*E*/ /*S*/ /*S*/ /* */ /* */ /* */
    //     0x73, 0x50, 0x79, 0x6d, 0x6d, 0x00, 0x00, 0x00,
    //     /* */ /* */ /* */ /* */ /* */ /* */ /* */ /* */
    //     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    //     /*b*/ /*u*/ /*t*/ /*t*/ /*o*/ /*n*/ /*S*/ /* */
    //     0x7c, 0x1c, 0x78, 0x78, 0x5c, 0x54, 0x6d, 0x00,
    //     /* */ /* */ /* */ /* */ /* */ /* */ /* */ /* */
    //     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    // };

    uint8_t promptText[] =
    {
        /*P*/ /*E*/ /*t*/ /*E*/ /*r*/ /* */ /* */ /* */
        0x73, 0x79, 0x78, 0x79, 0x50, 0x00, 0x00, 0x00,
        /* */ /* */ /* */ /* */ /* */ /* */ /* */ /* */
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        /*n*/ /*n*/ /*A*/ /*r*/ /*k/h*/ /* */ /* */ /* */
        0x54, 0x54, 0x77, 0x50, 0x74, 0x00, 0x00, 0x00,
        /* */ /* */ /* */ /* */ /* */ /* */ /* */ /* */
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    };

    static uint8_t block = 0;

    uint8_t textStartPos = (block / 4) << 3;
    for(uint8_t position = 0; position < 8; position++)
    {
        sendCommand(0x44);
        write(strobe, LOW);
        shiftOut(0xC0 + (position << 1));
        shiftOut(promptText[textStartPos + position]);
        write(strobe, HIGH);
    }

    block = (block + 1) % 16;

    uint8_t buttons = readButtons();

    for(uint8_t position = 0; position < 8; position++)
    {
        uint8_t mask = 0x1 << position;
        setLed(buttons & mask ? 1 : 0, position);
    }
}

uint8_t readButtons(void)
{
    uint8_t buttons = 0;
    write(strobe, LOW);
    shiftOut(0x42); // key scan (read buttons)

    DDRB &= ~(_BV(data)); // clear bit, direction = input

    for (uint8_t i = 0; i < 4; i++)
    {
        uint8_t v = shiftIn() << i;
        buttons |= v;
    }

    DDRB |= _BV(data); // set bit, direction = output
    write(strobe, HIGH);
    return buttons;
}

int main()
{
    typedef enum {COUNTING_MODE=0, SCROLL_MODE=1, BUTTON_MODE=2} mode_t;
    mode_t mode = COUNTING_MODE;
	uint8_t i = 0;

    setup();

    while (1) {
        switch(mode)
        {
            case COUNTING_MODE:
                mode += counting(); // keep on counting until done
                break;
            case SCROLL_MODE:
                mode += scroll(); // keep on scrolling until done
                break;
            case BUTTON_MODE:
                buttons();
                break;
        }

       _delay_ms(200);
    }

    return(0);
}
