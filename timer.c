// Tick counter — increments every timer interrupt
unsigned int timer_ticks = 0;

// Called by IRQ0 (timer interrupt) from idt.c
void timer_handler() {
    timer_ticks++;
}

// Return seconds running (timer fires ~18 times per second)
unsigned int timer_seconds() {
    return timer_ticks / 18;
}

// Return raw ticks
unsigned int timer_get_ticks() {
    return timer_ticks;
}
