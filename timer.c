// Define outb inline assembly function to communicate with hardware ports
static inline void outb(unsigned short port, unsigned char val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

unsigned int timer_ticks = 0;

void proc_tick();

void timer_handler() {
    timer_ticks++;
    proc_tick();
    
    // Send End of Interrupt (EOI) signal to the Master PIC
    outb(0x20, 0x20);
}

unsigned int timer_get_ticks() {
    return timer_ticks;
}

unsigned int timer_seconds() {
    return timer_ticks / 18;
}

// Get minutes since boot
unsigned int timer_minutes() {
    return timer_seconds() / 60;
}

// Get hours since boot
unsigned int timer_hours() {
    return timer_minutes() / 60;
}

// Get seconds part (0-59)
unsigned int timer_sec_part() {
    return timer_seconds() % 60;
}

// Get minutes part (0-59)
unsigned int timer_min_part() {
    return timer_minutes() % 60;
}

// Get hours part
unsigned int timer_hour_part() {
    return timer_hours();
}
