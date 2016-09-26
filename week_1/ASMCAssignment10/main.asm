
.include "m32def.inc"
.org 0

ldi zh, high(ASCII_TABLE << 1) ;define Z-pointer
ldi zl, low(ASCII_TABLE << 1)

ldi r16, 0x0
out DDRC, r16 ;PORTC input
ldi r16, 0xff
out DDRD, r16 ;PORTD output
BEGIN:
in r16, PINC
andi r16, 0b00000111 ;mask upper 5 bits
add zl, r16
lpm r17, z
out PORTD, r17
rjmp BEGIN

.org 20
ASCII_TABLE:
.DB '0','1','2','3','4','5','6','7'
