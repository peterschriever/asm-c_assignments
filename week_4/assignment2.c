#include <avr/io.h>
#define F_CPU 16E6
#include <util/delay.h>

void incrementDisplay();
void initDisplay();

int main(void) {
    initDisplay();
    while (1) {
        _delay_ms(100); // wait 100ms
        incrementDisplay();
    }
}

void initDisplay() {

}

void incrementDisplay() {
    
}
