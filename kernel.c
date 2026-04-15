// VGA text mode memory address
unsigned char *vga = (unsigned char *)0xB8000;
int cursor = 0;

void print_char(char c, unsigned char color) {
    vga[cursor * 2]     = c;
    vga[cursor * 2 + 1] = color;
    cursor++;
}

void print_string(char *str, unsigned char color) {
    int i = 0;
    while (str[i] != '\0') {
        print_char(str[i], color);
        i++;
    }
}

void print_newline() {
    cursor = cursor + (80 - (cursor % 80));
}

void clear_screen() {
    int i = 0;
    while (i < 80 * 25) {
        vga[i * 2]     = ' ';
        vga[i * 2 + 1] = 0x0F;
        i++;
    }
    cursor = 0;
}

void backspace() {
    if (cursor > 0) {
        cursor--;
        vga[cursor * 2]     = ' ';
        vga[cursor * 2 + 1] = 0x0F;
    }
}

// Print an integer number
void print_int(int num, unsigned char color) {
    if (num == 0) {
        print_char('0', color);
        return;
    }

    char digits[12];
    int i = 0;

    // Handle negative numbers
    if (num < 0) {
        print_char('-', color);
        num = -num;
    }

    // Extract digits in reverse
    while (num > 0) {
        digits[i] = '0' + (num % 10);
        num = num / 10;
        i++;
    }

    // Print digits in correct order
    while (i > 0) {
        i--;
        print_char(digits[i], color);
    }
}

// Print a hex number like 0xB8000
void print_hex(unsigned int num, unsigned char color) {
    char hex_chars[] = "0123456789ABCDEF";
    print_string("0x", color);

    int i;
    for (i = 7; i >= 0; i--) {
        unsigned int nibble = (num >> (i * 4)) & 0xF;
        print_char(hex_chars[nibble], color);
    }
}

// Declare functions from other files
char keyboard_read();
void shell_handle_key(char c);
void idt_init();

// Kernel main
void kernel_main() {
    idt_init();    
    clear_screen();

    print_string("DeepithOS v0.1", 0x0B);
    print_newline();
    print_string("Kernel Loaded Successfully!", 0x0A);
    print_newline();
    print_newline();
    print_string("> ", 0x0E);

while (1) {
        char c = keyboard_read();
        if (c == 0) continue;

        if (c == '\b') {
            backspace();
        } else if (c != '\n') {
            print_char(c, 0x0F);
        }

        shell_handle_key(c);
    }
}
