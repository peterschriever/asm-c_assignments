volatile int gv_counter;
init_timer();
// start counting
TCNT1 = 0;

// ... wait some time ....

// read value timer1
gv_counter = TCNT1;
// display tomer value
display_value(gv_counter);
