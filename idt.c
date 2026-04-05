// IDT Entry structure — 8 bytes each
struct idt_entry {
    unsigned short base_low;    // lower 16 bits of handler address
    unsigned short selector;    // code segment selector
    unsigned char  zero;        // always zero
    unsigned char  flags;       // type and attributes
    unsigned short base_high;   // upper 16 bits of handler address
} __attribute__((packed));

// IDT pointer structure
struct idt_ptr {
    unsigned short limit;
    unsigned int   base;
} __attribute__((packed));

// 256 IDT entries (one per interrupt)
struct idt_entry idt[256];
struct idt_ptr   idtp;

// Declare assembly function
extern void idt_load(struct idt_ptr *ptr);

// Set one IDT entry
void idt_set_gate(unsigned char num, unsigned int base, unsigned short sel, unsigned char flags) {
    idt[num].base_low  = (base & 0xFFFF);
    idt[num].base_high = (base >> 16) & 0xFFFF;
    idt[num].selector  = sel;
    idt[num].zero      = 0;
    idt[num].flags     = flags;
}

// Forward declare handlers
void isr0();
void isr1();
void irq1();

// Print functions from kernel.c
void print_string(char *str, unsigned char color);
void print_newline();

// Exception handler called from assembly
void isr_handler(int num) {
    print_string("  CPU Exception: ", 0x0C);
    if (num == 0) print_string("Divide by Zero!", 0x0C);
    if (num == 1) print_string("Debug!", 0x0C);
    print_newline();
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

    // Set exception handlers
    idt_set_gate(0, (unsigned int)isr0, 0x08, 0x8E);
    idt_set_gate(1, (unsigned int)isr1, 0x08, 0x8E);

    // Load IDT
    idt_load(&idtp);
}
