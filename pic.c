// PIC ports
#define PIC1_CMD  0x20
#define PIC1_DATA 0x21
#define PIC2_CMD  0xA0
#define PIC2_DATA 0xA1

// port_read and port_write live in keyboard.c
unsigned char port_read(unsigned short port);

// Write to hardware port
void port_write(unsigned short port, unsigned char data) {
    __asm__("outb %0, %1" : : "a"(data), "Nd"(port));
}

// Small delay needed between PIC commands
void pic_wait() {
    __asm__("outb %%al, $0x80" : : "a"(0));
}

// Remap PIC so IRQs don't clash with CPU exceptions
void pic_remap() {
    unsigned char mask1 = port_read(PIC1_DATA);
    unsigned char mask2 = port_read(PIC2_DATA);

    port_write(PIC1_CMD,  0x11); pic_wait();
    port_write(PIC2_CMD,  0x11); pic_wait();

    port_write(PIC1_DATA, 0x20); pic_wait();
    port_write(PIC2_DATA, 0x28); pic_wait();

    port_write(PIC1_DATA, 0x04); pic_wait();
    port_write(PIC2_DATA, 0x02); pic_wait();

    port_write(PIC1_DATA, 0x01); pic_wait();
    port_write(PIC2_DATA, 0x01); pic_wait();

    port_write(PIC1_DATA, mask1);
    port_write(PIC2_DATA, mask2);
}

// Enable only keyboard IRQ (IRQ1)
void pic_enable_keyboard() {
    port_write(PIC1_DATA, 0xFD);
    port_write(PIC2_DATA, 0xFF);
}

// Send End of Interrupt signal
void pic_eoi() {
    port_write(PIC1_CMD, 0x20);
}
