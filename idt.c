// IDT Entry structure — 8 bytes each
struct idt_entry {
    unsigned short base_low;
    unsigned short selector;
    unsigned char  zero;
    unsigned char  flags;
    unsigned short base_high;
} __attribute__((packed));

// IDT pointer structure
struct idt_ptr {
    unsigned short limit;
    unsigned int   base;
} __attribute__((packed));

// 256 IDT entries
struct idt_entry idt[256];
struct idt_ptr   idtp;

// Assembly functions
extern void idt_load(struct idt_ptr *ptr);

// PIC functions from pic.c
void pic_remap();
void pic_enable_keyboard();
void pic_eoi();

// Print functions from kernel.c
void print_string(char *str, unsigned char color);
void print_newline();

// Keyboard handler from keyboard.c
void keyboard_handler();
void timer_handler();

// ISR and IRQ handler declarations
void isr0();
void isr1();
void irq0();
void irq1();

// Set one IDT entry
void idt_set_gate(unsigned char num, unsigned int base,
                  unsigned short sel, unsigned char flags) {
    idt[num].base_low  = (base & 0xFFFF);
    idt[num].base_high = (base >> 16) & 0xFFFF;
    idt[num].selector  = sel;
    idt[num].zero      = 0;
    idt[num].flags     = flags;
}

// CPU exception handler (called from idt.asm)
void isr_handler(int num) {
    print_string("  CPU Exception: ", 0x0C);
    if (num == 0) print_string("Divide by Zero!", 0x0C);
    if (num == 1) print_string("Debug!", 0x0C);
    print_newline();
}

// Hardware IRQ handler (called from idt.asm)
void irq_handler(int num) {
    if (num == 32) timer_handler();
    if (num == 33) keyboard_handler();
    pic_eoi();
}

// Initialize the IDT
void idt_init() {
    idtp.limit = (sizeof(struct idt_entry) * 256) - 1;
    idtp.base  = (unsigned int)&idt;

    // Clear all entries
    int i = 0;
    while (i < 256) {
        idt_set_gate(i, 0, 0x08, 0x8E);
        i++;
    }

    // CPU exception handlers
    idt_set_gate(0, (unsigned int)isr0, 0x08, 0x8E);
    idt_set_gate(1, (unsigned int)isr1, 0x08, 0x8E);

    // Hardware IRQ handlers (remapped to 32+)
    idt_set_gate(32, (unsigned int)irq0, 0x08, 0x8E);
    idt_set_gate(33, (unsigned int)irq1, 0x08, 0x8E);

    // Remap PIC to avoid exception overlap conflicts
    pic_remap();
    
    // Unmask BOTH IRQ0 (Timer) and IRQ1 (Keyboard) on the Master PIC data port.
    // 0xFC = 11111100b -> Bits 0 and 1 set to 0 (Unmasked/Enabled)
    __asm__ volatile("outb %0, $0x21" : : "a"((unsigned char)0xFC));

    // Load IDT into the CPU register and enable interrupts globally
    idt_load(&idtp);
    __asm__("sti");
}
